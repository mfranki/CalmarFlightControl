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
#define UART_MAX_MESSAGE_SIZE 200
#endif

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes uart
 *
 * @param [in] uh - handle to uart periph
 * @return true if success
 */
bool UartInit(UART_HandleTypeDef *uh);

/**@brief writes data to uart, formatting the same as in printf
 *
 * @param format
 * @return true if success
 */
bool UartWrite(char *format, ...);

