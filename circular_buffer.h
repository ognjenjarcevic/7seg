/** 
 * @file circular_buffer.h
 * 
 * @brief Pseudo circular buffer used for display driver.
 * 
 * @note Requires a lot of manual management.
 * Members are added as they would be to a non-circular buffer.
 * Only reading of the values is done circularly by advancing the read pointers.
 * 
 * @authors Ognjen Jarcevic RA99/2020, Lazar Vranjes RA19/2020
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

#define CIRC_BUFF_MAX_OUT_LEN 128

struct CircularBuffer
{
    /** @brief Contains values of characters that are to be displayed */
    uint8_t data[CIRC_BUFF_MAX_OUT_LEN];

    /** @brief Current buffer length */
    uint8_t len;

    /** @brief Read pointer. Points to the member that is to be displayed on the first slot of the 7seg display */
    uint8_t* firstDisplayed;
};

/** @brief Initializes the circular buffer */
void circular_buffer_init(struct CircularBuffer* cb);

/** @brief Moves the passed pointer one place forward */
void circular_buffer_advance(struct CircularBuffer* cb, uint8_t** dataPtr);


#endif //CIRCULAR_BUFFER_H