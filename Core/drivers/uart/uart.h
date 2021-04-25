/*****************************************************************************
 * @file /CALMAR/Core/driver/uart/uart.h
 *
 * @brief 
 * 
 * @author Michal Frankiewicz
 * @date May 5, 2020
 *
 ****************************************************************************/
#pragma once 

#include "main.h"

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

#ifndef UART_MAX_MESSAGE_SIZE
#define UART_MAX_MESSAGE_SIZE 100
#endif

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

bool UartInit(UART_HandleTypeDef *uh);

bool UartWrite(char *format, ...);

