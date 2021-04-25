/*****************************************************************************
 * @file /CALMAR/Core/driver/uart/uart.c
 *
 * @brief 
 * 
 * @author Michal Frankiewicz
 * @date May 5, 2020
 *
 ****************************************************************************/

#include "drivers/uart/uart.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static UART_HandleTypeDef *uartHandle;

static char messageTooLongErrMsg[] = "Message too long\r\n";

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool UartInit(UART_HandleTypeDef *uh)
{
    uartHandle = uh;
    return true;
}

bool UartWrite(char *format, ...)
{
    if(strlen(format) > UART_MAX_MESSAGE_SIZE)
    {
        if(HAL_UART_Transmit(uartHandle, (uint8_t*)messageTooLongErrMsg, strlen(messageTooLongErrMsg), 1000) != HAL_OK)
        {
            return false;
        }
        return false;
    }

    va_list aptr;
    va_start(aptr, format);
    char buffer[UART_MAX_MESSAGE_SIZE];
    uint32_t msgSize = vsprintf(buffer, format, aptr);
    va_end(aptr);

    if(HAL_UART_Transmit(uartHandle, (uint8_t*)buffer, msgSize, 1000) != HAL_OK)
    {
        return false;
    }
    return true;
}


/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

