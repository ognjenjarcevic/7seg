#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h> //for sleep
#include <time.h> //msleep
#include "display.h"
#include "bcm2835.h"
#include "max7219_types.h"
#include "circular_buffer.h"
#include "bcm_bitbang.h"


/** @brief Uses bcm2835 library with SPI pins and functions */
#define USE_BCM2835_SPI_LIB 0

/** @brief Uses bcm2835 library GPIO functions to bitbang the MAX2019 spi */
#define USE_BCM2835_BITBANG_LIB 0

/** @brief Bitbangs with Linux kernel and GPIO pins */
#define USE_GPIO_BITBANG_LIB 1

#define DEV_FN "/dev/gpio_bitbang"


/**
 * @brief Helper macro for error handling
 *
 * @note Guaranteed to only perform one evaluation of `status`
 */
#define DISPLAY_FAIL_UNLESS_OK(status) \
    do                                \
    {                                 \
        int temp = (status);          \
        if (1 != temp)                \
        {                             \
            return temp;              \
        }                             \
    } while (0)

#define DISPLAY_MAX_STR_LEN 128
#define DISPLAY_INSTR_LEN 2


enum DisplayState
{
    DISPLAY_STATE_UNINITIALIZED,
    DISPLAY_STATE_INITIALIZED
};

struct DisplayContext
{
    /** @brief Contains current device state*/
    enum DisplayState state;
    /** @brief Current/last SPI instruction sent to the display */
    char instr[DISPLAY_INSTR_LEN];
    /** @brief Stores user input*/
    char userInput[DISPLAY_MAX_STR_LEN];
    /** @brief Buffer used to store characters to be displayed */
    struct CircularBuffer outBuff;
    /** @brief Read/write pointer of `outBuff` */
    int outBuffIndex;
    /** @brief Tells if current character's dot LED displays a dot */
    bool dotDisplayed;
    /** @brief Thread that updates the display*/
    pthread_t updateThread;
    /** @brief Contains the information if first advertisement is being displayed */
    bool firstTime;
    /** @brief Exit command that user needs to write to terminate the program: default is "exit"*/
    char exitCommand[5];


    /** @brief File descriptor for the gpio_bitbang driver*/
    int gpio_fd;
};

static struct DisplayContext context = {
    .state = DISPLAY_STATE_UNINITIALIZED,
    .outBuffIndex = -1,
    .instr = {0},
    .dotDisplayed = false,
    .userInput = {0},
    .firstTime = true,
    .exitCommand = "exit"
};

static void display_spi_write(char reg, char val)
{
#if USE_BCM2835_SPI_LIB

    context.instr[0] = reg;
    context.instr[1] = val;
    bcm2835_spi_writenb(context.instr, DISPLAY_INSTR_LEN);

#elif USE_GPIO_BITBANG_LIB
    uint16_t instr = ((uint16_t)reg<<8) | (uint16_t)val;
    printf("bbb: %x|%x\n", reg, val);
    int status = write(context.gpio_fd, (char*)&instr, sizeof(instr));

#elif USE_BCM2835_BITBANG_LIB
    uint16_t bits = ((uint16_t)reg<<8) | (uint16_t)val;
    //bitbangbits
    printf("bbb: %x|%x\n", reg, val);

    bcm2835_gpio_set(BCM_BITBANG_LOAD_PIN);
    usleep(BCM_BITBANG_DELAY_USEC);

    for (int i = 16; i > 0; i--)
    {
        // Calculate bitmask, MSB first, LSB last
        unsigned short mask = 1 << (i - 1); 

        // Write a bit to DIN while the CLK is cleared
        bcm2835_gpio_clr(BCM_BITBANG_CLK_PIN);
        usleep(BCM_BITBANG_DELAY_USEC);

        // Write the current data bit
        if (bits & mask)
        {
            bcm2835_gpio_set(BCM_BITBANG_DIN_PIN);
        }
        else
        {
            bcm2835_gpio_clr(BCM_BITBANG_DIN_PIN);
        }
        usleep(BCM_BITBANG_DELAY_USEC);

        // Processes the bit on the rising edge
        bcm2835_gpio_set(BCM_BITBANG_CLK_PIN);
        usleep(BCM_BITBANG_DELAY_USEC);
    }

    // Stops the data input
    bcm2835_gpio_clr(BCM_BITBANG_LOAD_PIN);
    usleep(BCM_BITBANG_DELAY_USEC);
    bcm2835_gpio_set(BCM_BITBANG_LOAD_PIN);
    usleep(BCM_BITBANG_DELAY_USEC);

#endif
}

int display_init()
{   
    //Pin initialization
    int status = 0;

#if USE_BCM2835_SPI_LIB

    status = bcm2835_init();
    DISPLAY_FAIL_UNLESS_OK(status);
    printf("bcm2835 init\n");
    status = bcm2835_spi_begin();
    DISPLAY_FAIL_UNLESS_OK(status);
    printf("bcm2835 init\n");

    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32768);

#elif USE_GPIO_BITBANG_LIB

    context.gpio_fd = open(DEV_FN, O_RDWR);
	if(context.gpio_fd < 0)
    {
		printf("ERROR: \"%s\" not opened!\n", DEV_FN);
		return 4;
	}
    printf("gpio_bitbang kernel driver opened, %d!\n", context.gpio_fd);
    DISPLAY_FAIL_UNLESS_OK(status);

#elif USE_BCM2835_BITBANG_LIB

    status = bcm2835_init();
    DISPLAY_FAIL_UNLESS_OK(status);

    // Configures the neccessary GPIO pins into output pins
    bcm2835_gpio_fsel(BCM_BITBANG_DIN_PIN, BCM2835_GPIO_FSEL_OUTP); //RPI_V2_GPIO_P1_11
    bcm2835_gpio_fsel(BCM_BITBANG_LOAD_PIN, BCM2835_GPIO_FSEL_OUTP); //RPI_V2_GPIO_P1_13
    bcm2835_gpio_fsel(BCM_BITBANG_CLK_PIN, BCM2835_GPIO_FSEL_OUTP); //RPI_V2_GPIO_P1_15

#endif

    display_spi_write(REG_SCAN_LIMIT, SCAN_LIMIT_7);
    display_spi_write(REG_DECODE_MODE, DECODE_OFF);
    display_spi_write(REG_DISPLAY_TEST, TEST_MODE_OFF);
    display_spi_write(REG_INTENSITY, INTENSITY_31_32);
    display_spi_write(REG_SHUTDOWN, SHUTDOWN_5V);

    //Context initialization
    circular_buffer_init(&context.outBuff);
    context.state = DISPLAY_STATE_INITIALIZED;
    return 0;
}

void display_destroy()
{
    printf("Quitting..\n");
    pthread_cancel(context.updateThread);
    display_clear();

#if USE_GPIO_BITBANG_LIB
    close(context.gpio_fd);
#endif
}

static int display_parseChar(const char* input)
{
    if (*input == '.' || *input == ',')
    {
        // Compress
        // Adds dot to the 7seg display

        //If the first character is dot
        if(context.outBuffIndex == -1)
        {
            context.outBuffIndex = 0;
        }

        context.outBuff.data[context.outBuffIndex] |= CHAR_DOT;
        context.outBuffIndex++;
        context.outBuff.len++;
        context.dotDisplayed = true;
        return 0;
    }

    //else

    if(context.dotDisplayed == false)
    {
        context.outBuffIndex++;
        context.outBuff.len++;
    }
    context.dotDisplayed = false;

    switch (*input)
    {
        case '0': context.outBuff.data[context.outBuffIndex] = CHAR_ZERO; break;
        case '1': context.outBuff.data[context.outBuffIndex] = CHAR_ONE; break;
        case '2': context.outBuff.data[context.outBuffIndex] = CHAR_TWO; break;
        case '3': context.outBuff.data[context.outBuffIndex] = CHAR_THREE; break;
        case '4': context.outBuff.data[context.outBuffIndex] = CHAR_FOUR; break;
        case '5': context.outBuff.data[context.outBuffIndex] = CHAR_FIVE; break;
        case '6': context.outBuff.data[context.outBuffIndex] = CHAR_SIX; break;
        case '7': context.outBuff.data[context.outBuffIndex] = CHAR_SEVEN; break;
        case '8': context.outBuff.data[context.outBuffIndex] = CHAR_EIGHT; break;
        case '9': context.outBuff.data[context.outBuffIndex] = CHAR_NINE; break;

        case 'A': context.outBuff.data[context.outBuffIndex] = CHAR_A; break;
        case 'a': context.outBuff.data[context.outBuffIndex] = CHAR_A; break;
        case 'B': context.outBuff.data[context.outBuffIndex] = CHAR_B_UPPER; break;
        case 'b': context.outBuff.data[context.outBuffIndex] = CHAR_B_LOWER; break;
        case 'C': context.outBuff.data[context.outBuffIndex] = CHAR_C_UPPER; break;
        case 'c': context.outBuff.data[context.outBuffIndex] = CHAR_C_LOWER; break;
        case 'D': context.outBuff.data[context.outBuffIndex] = CHAR_D; break;
        case 'd': context.outBuff.data[context.outBuffIndex] = CHAR_D; break;
        case 'E': context.outBuff.data[context.outBuffIndex] = CHAR_E; break;
        case 'e': context.outBuff.data[context.outBuffIndex] = CHAR_E; break;
        case 'F': context.outBuff.data[context.outBuffIndex] = CHAR_F; break;
        case 'f': context.outBuff.data[context.outBuffIndex] = CHAR_F; break;
        case 'G': context.outBuff.data[context.outBuffIndex] = CHAR_G_UPPER; break;
        case 'g': context.outBuff.data[context.outBuffIndex] = CHAR_G_LOWER; break;
        case 'H': context.outBuff.data[context.outBuffIndex] = CHAR_H_UPPER; break;
        case 'h': context.outBuff.data[context.outBuffIndex] = CHAR_H_LOWER; break;
        case 'I': context.outBuff.data[context.outBuffIndex] = CHAR_I_UPPER; break;
        case 'i': context.outBuff.data[context.outBuffIndex] = CHAR_I_LOWER; break;
        case 'J': context.outBuff.data[context.outBuffIndex] = CHAR_J_UPPER; break;
        case 'j': context.outBuff.data[context.outBuffIndex] = CHAR_J_LOWER; break;
        case 'L': context.outBuff.data[context.outBuffIndex] = CHAR_L_UPPER; break;
        case 'l': context.outBuff.data[context.outBuffIndex] = CHAR_L_LOWER; break;
        case 'N': context.outBuff.data[context.outBuffIndex] = CHAR_N_UPPER; break;
        case 'n': context.outBuff.data[context.outBuffIndex] = CHAR_N_LOWER; break;
        case 'O': context.outBuff.data[context.outBuffIndex] = CHAR_O_UPPER; break;
        case 'o': context.outBuff.data[context.outBuffIndex] = CHAR_O_LOWER; break;
        case 'P': context.outBuff.data[context.outBuffIndex] = CHAR_P; break;
        case 'p': context.outBuff.data[context.outBuffIndex] = CHAR_P; break;
        case 'Q': context.outBuff.data[context.outBuffIndex] = CHAR_Q; break;
        case 'q': context.outBuff.data[context.outBuffIndex] = CHAR_Q; break;
        case 'R': context.outBuff.data[context.outBuffIndex] = CHAR_R; break;
        case 'r': context.outBuff.data[context.outBuffIndex] = CHAR_R; break;
        case 'S': context.outBuff.data[context.outBuffIndex] = CHAR_S; break;
        case 's': context.outBuff.data[context.outBuffIndex] = CHAR_S; break;
        case 'T': context.outBuff.data[context.outBuffIndex] = CHAR_T; break;
        case 't': context.outBuff.data[context.outBuffIndex] = CHAR_T; break;
        case 'U': context.outBuff.data[context.outBuffIndex] = CHAR_U_UPPER; break;
        case 'u': context.outBuff.data[context.outBuffIndex] = CHAR_U_LOWER; break;
        case 'Y': context.outBuff.data[context.outBuffIndex] = CHAR_Y; break;
        case 'y': context.outBuff.data[context.outBuffIndex] = CHAR_Y; break;

        case ' ': context.outBuff.data[context.outBuffIndex] = CHAR_SPACE; break;
        case '-': context.outBuff.data[context.outBuffIndex] = CHAR_MINUS; break;
        case '_': context.outBuff.data[context.outBuffIndex] = CHAR_LOW_LINE; break;
        case '\"': context.outBuff.data[context.outBuffIndex] = CHAR_QUOTATION_MARK; break;
        case '\'': context.outBuff.data[context.outBuffIndex] = CHAR_APOSTROPHE; break;
        case '=': context.outBuff.data[context.outBuffIndex] = CHAR_EQUAL_SIGN; break;

        default: context.outBuff.data[context.outBuffIndex] = CHAR_EMPTY; return -1;
    }
    return 0;
}

static void *display_updateDigits(void* parm)
{
    //init check
    while (1)
    {
        uint8_t* digitRunner = context.outBuff.firstDisplayed;
        
        display_spi_write(REG_DIGIT_7, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_6, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_5, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_4, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_3, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_2, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_1, *digitRunner);

        circular_buffer_advance(&context.outBuff, &digitRunner);
        display_spi_write(REG_DIGIT_0, *digitRunner);


        circular_buffer_advance(&context.outBuff, &context.outBuff.firstDisplayed);

        //printf(".");
        usleep(500000);
        //sleep(1);
    }
}

static void display_initUpdateThread()
{   
    int status = pthread_create(&context.updateThread, NULL, display_updateDigits, NULL);

    if(status < 0)
        printf("pthread_create failed");
    else
        printf("pthread_create success\n");

    status = pthread_detach(context.updateThread);

    if(status < 0)
        printf("pthread_detach failed");
    else
        printf("pthread_detach success\n");
}

int display_advertisement()
{
    // Prepares the input buffer for incoming data
    for(int i = 0; i < DISPLAY_MAX_STR_LEN; i++)
    {
        context.userInput[i] = '\0';
    }

    // User input, fgets waits
    printf("Input advertisement text: ");
    fflush(stdin);
    fgets(context.userInput, DISPLAY_MAX_STR_LEN, stdin);
    context.userInput[strlen(context.userInput)-1] = '\0';

    //printf("Exit command check\n");
    //Exit if command is issued
    if (strcmp(context.userInput, context.exitCommand) == 0)
    {
        //printf("Exit command issued\n");
        delay(10);
        display_destroy();
        return -DISPLAY_EXIT_CODE;
    }
    //printf("Regular display command issued\n");
    // Prepares the display and output buffer
    display_clear();
    context.outBuffIndex = -1;
    context.dotDisplayed = false;
    circular_buffer_init(&context.outBuff);
    for (int i = 0; i < strlen(context.userInput); i++)
    {
        display_parseChar(&context.userInput[i]);
    }

    //Turns on the advertisement if not turned on
    if(context.firstTime == true)
    {
        display_initUpdateThread();
    }
    context.firstTime = false;

    return 0;
}

void display_printTest()
{
    display_spi_write(REG_DIGIT_7, CHAR_ONE | CHAR_DOT);
    display_spi_write(REG_DIGIT_6, CHAR_TWO |CHAR_DOT);
    display_spi_write(REG_DIGIT_5, CHAR_THREE |CHAR_DOT);
    display_spi_write(REG_DIGIT_4, CHAR_FOUR |CHAR_DOT);
    display_spi_write(REG_DIGIT_3, CHAR_FIVE |CHAR_DOT);
    display_spi_write(REG_DIGIT_2, CHAR_SIX |CHAR_DOT);
    display_spi_write(REG_DIGIT_1, CHAR_SEVEN |CHAR_DOT);
    display_spi_write(REG_DIGIT_0, CHAR_EIGHT |CHAR_DOT);
}

void display_clear()
{
    display_spi_write(REG_DIGIT_0, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_1, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_2, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_3, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_4, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_5, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_6, CHAR_EMPTY);
    display_spi_write(REG_DIGIT_7, CHAR_EMPTY);
    //printf("Display cleared\n");
}
