/*****************************************************************************
 * @file /CalmarFlightController/Core/app/deviceManager/deviceManager.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date May 24, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes device
 *
 * @param [in] adcHandle
 * @param [in] spiBMXHandle
 * @param [in] timBuzzerHandle
 * @param [in] uartDebugHandle
 */
void DeviceManagerInit(ADC_HandleTypeDef* adcHandle,
                       SPI_HandleTypeDef* spiBMXHandle,
                       TIM_HandleTypeDef* timBuzzerHandle,
                       UART_HandleTypeDef* uartDebugHandle);

