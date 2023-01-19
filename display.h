/** 
 * @file display.h
 * @brief MAX7219/MAX7221 RPI2 display driver.
 * 
 * 
 * @note Requires Linux kernel and "sudo su" to run.
 * Wiring guide:
 *___________hw179 - bcm2835___________
 *
 *             VCC - +5V
 *             GND - GND
 *______SPI pins for bcm2835 spi:______
 *             DIN - SPI0 MOSI (PIN 19)
 *              CS - SPI0 CE0 (PIN 24)
 *             CLK - SPI0 SCLK (PIN 23)
 * 
 *________GPIO pins for bitbang________
 *
 *             DIN - GPIO 17 (PIN 11)
 *         CS/LOAD - GPIO 27 (PIN 13)
 *             CLK - GPIO 22 (PIN 15)
 * 
 * @authors Ognjen Jarcevic RA99/2020, Lazar Vranjes RA19/2020
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_EXIT_CODE 150

#define BITBANG_
/**
 * @brief Initializes the display
 * 
 * @retval 0 on success or an error code
*/
int display_init();

/**
 * @brief Deinitializes the display, stops the advertisment
 * Display can't be used again until program termination
 * 
 * @retval 0 on success or an error code
*/
void display_destroy();

/** @brief Requests string input through console, and displays it, as a scrolling advertisement. 
 *  If an advertisement is already displayed, replaces the current one.
 * 
 *  @retval 0 on success, -DISPLAY_EXIT_CODE on user exit command
*/
int display_advertisement();

/** @brief Displays "1.2.3.4.5.6.7.8." */
void display_printTest();

/** @brief Clears the display. Doesn't stop the advertisement. */
void display_clear();


#endif //DISPLAY_H