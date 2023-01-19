#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>

void circular_buffer_init(struct CircularBuffer* cb)
{
    for(uint8_t i = 0; i < CIRC_BUFF_MAX_OUT_LEN; i++)
    {
        cb->data[i] = (uint8_t) 0;
    }

    cb->len = (uint8_t) 0;
    cb->firstDisplayed = cb->data;
}

void circular_buffer_advance(struct CircularBuffer* cb, uint8_t** dataPtr)
{
    //checks if the param pointer points to the array, ensuring memory safety
    if(*dataPtr < cb->data || *dataPtr > (cb->data + cb->len))
    {
        //printf("circular_buffer_advance failed\n");
        return;
    }

    //point to the first member if on last
    if (*dataPtr == &cb->data[(cb->len)-1])
    {
        //printf("circular_buffer_advance return to start\n");
        *dataPtr = cb->data;
        return;
    }

    //else: points to the next
    (*dataPtr)++;
    //printf("circular_buffer_advanced to %d\n", *dataPtr);
    return;
}
