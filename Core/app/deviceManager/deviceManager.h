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

typedef enum{
    DEVICE_INITIALIZATION,  /// 0
    DEVICE_STANDBY,         /// 1
    DEVICE_CALIBRATION,     /// 2
    DEVICE_SETTINGS,        /// 3
    DEVICE_FLIGHT,          /// 4
    DEVICE_HOMING,          /// 5
    DEVICE_ERROR
}deviceOperatingModes_t;

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


/**@brief getter for current operating mode
 *
 * @return current device operating mode
 */
deviceOperatingModes_t DeviceManagerGetOperatingMode();
