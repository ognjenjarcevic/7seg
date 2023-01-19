/** 
 * @file max7219_types.h
 * @brief Types for MAX7219/MAX7221 platform-independent driver
 * 
 * @authors Ognjen Jarcevic RA99/2020, Lazar Vranjes RA19/2020
 */

#ifndef MAX7219_TYPES_H
#define MAX7219_TYPES_H


/** @brief Register map */
typedef enum Register 
{
    // Register for cascading mutliple MAX7219s
    REG_NO_OP = (0x00),

    // Digit registers
    REG_DIGIT_0 = (0x01), //< Rightmost digit
    REG_DIGIT_1 = (0x02),
    REG_DIGIT_2 = (0x03),
    REG_DIGIT_3 = (0x04),
    REG_DIGIT_4 = (0x05),
    REG_DIGIT_5 = (0x06),
    REG_DIGIT_6 = (0x07),
    REG_DIGIT_7 = (0x08), //< Leftmost digit

    // Control registers
    REG_DECODE_MODE = (0x09),
    REG_INTENSITY = (0x0A),
    REG_SCAN_LIMIT = (0x0B),
    REG_SHUTDOWN = (0x0C),
    REG_DISPLAY_TEST = (0x0F),
} Register;

/** @brief Every character that can be displayed in a single digit */
typedef enum Character
{
    CHAR_ZERO = (0x7E),
    CHAR_ONE = (0x30),
    CHAR_TWO = (0x6D),
    CHAR_THREE = (0x79),
    CHAR_FOUR = (0x33),
    CHAR_FIVE = (0x5B),
    CHAR_SIX = (0x5F),
    CHAR_SEVEN = (0x70),
    CHAR_EIGHT = (0x7F),
    CHAR_NINE = (0x7B),

    CHAR_A = (0x77),
    CHAR_B_UPPER = (0x7F),
    CHAR_B_LOWER = (0x1F),
    CHAR_C_UPPER = (0x4E),
    CHAR_C_LOWER = (0x0D),
    CHAR_D = (0x3D),
    CHAR_E = (0x4F),
    CHAR_F = (0x47),
    CHAR_G_UPPER = (0x5E),
    CHAR_G_LOWER = (0x7B),
    CHAR_H_UPPER = (0x37),
    CHAR_H_LOWER = (0x17),
    CHAR_I_UPPER = (0x30), 
    CHAR_I_LOWER = (0x50),
    CHAR_J_UPPER = (0x78),
    CHAR_J_LOWER = (0x58),
    CHAR_L_UPPER = (0x0E),
    CHAR_L_LOWER = (0x06),
    CHAR_N_UPPER = (0x76),
    CHAR_N_LOWER = (0x15),
    CHAR_O_UPPER = (0x7E),
    CHAR_O_LOWER = (0x1D),
    CHAR_P = (0x67),
    CHAR_Q = (0x73),
    CHAR_R = (0x05),
    CHAR_S = (0x5B),
    CHAR_T = (0x0F),
    CHAR_U_UPPER = (0x3E),
    CHAR_U_LOWER = (0x1C),
    CHAR_Y = (0x33),

    CHAR_EMPTY = (0x00),
    CHAR_SPACE = (0x00),
    CHAR_COMMA = (0x80),
    CHAR_DOT = (0x80),
    CHAR_MINUS = (0x01),
    CHAR_HYPHEN = (0X01),
    CHAR_LOW_LINE = (0x08),
    CHAR_QUOTATION_MARK =(0x22),
    CHAR_APOSTROPHE = (0x02),
    CHAR_EQUAL_SIGN = (0x09)
} Character;

/**
 * @brief Intensity register settings. 
 * Display brightness, digitally controlled by an internal pulse-width modulator.
 */
typedef enum Intensity
{
    INTENSITY_1_32 = (0x00),
    INTENSITY_3_32 = (0x01),
    INTENSITY_5_32 = (0x02),
    INTENSITY_7_32 = (0x03),
    INTENSITY_9_32 = (0x04),
    INTENSITY_11_32 = (0x05),
    INTENSITY_13_32 = (0x06),
    INTENSITY_15_32 = (0x07),
    INTENSITY_17_32 = (0x08),
    INTENSITY_19_32 = (0x09),
    INTENSITY_21_32 = (0x0A),
    INTENSITY_23_32 = (0x0B),
    INTENSITY_25_32 = (0x0C),
    INTENSITY_27_32 = (0x0D),
    INTENSITY_29_32 = (0x0E),
    INTENSITY_31_32 = (0x0F)
} Intensity;

/**
 * @brief Scan-Limit register settings.
 * The scan-limit register sets how tmany digits are displayed, from 1 to 8.
 */
typedef enum ScanLimit 
{
    SCAN_LIMIT_0 = (0x00),
    SCAN_LIMIT_1 = (0x01),
    SCAN_LIMIT_2 = (0x02),
    SCAN_LIMIT_3 = (0x03),
    SCAN_LIMIT_4 = (0x04),
    SCAN_LIMIT_5 = (0x05),
    SCAN_LIMIT_6 = (0x06),
    SCAN_LIMIT_7 = (0x07)
} ScanLimit;


/**
 * @brief Display-Test register settings.
 * In test mode, every LED is turned on at maximum brightnes
*/
typedef enum TestMode 
{
    TEST_MODE_OFF = (0x00),
    TEST_MODE_ON = (0x01)
} TestMode;

/** @brief Shutdown register settings. */
typedef enum ShutdownMode 
{
    SHUTDOWN_0V = (0x00), //< Shuts the device off
    SHUTDOWN_5V = (0x01)  //< Turns the device on
} ShutdownMode;

/** @brief Decode-Mode register settings. */
typedef enum DecodeMode 
{
    DECODE_OFF = (0x00),
    DECODE_ON_1 = (0x01),
    DECODE_ON_2 = (0x0F),
    DECODE_ON_3 = (0xFF)
} DecodeMode;

#endif //MAX7219_TYPES_H
