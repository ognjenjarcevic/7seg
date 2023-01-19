#include "display.h"
#include <stdio.h>

int main(void)
{
    int status = display_init();
    if(status != 0)
    {
        printf("display_init failed\n");
        return status;
    }
    display_clear();
    //display_printTest();
    printf("type \"exit\" to quit the program\n");
    while(status == 0)
    {  
        status = display_advertisement();
    }
    return 0;
}