/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/batteryStatus/batteryStatus.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef enum {
    BATTERY_OVERVOLTAGE,
    BATTERY_OK,
    BATTERY_LOW,
    BATTERY_DEPLEATED,
    BATTERY_UNDERVOLTAGE
}batteryStatus_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initiaizes module,
 *        checks if battery voltage is in right range and detects battery cell count
 * @return true if voltage is correct
 */
bool BatteryStatusInit();

/**@brief getter for battery status
 *
 * @return battery status
 */
batteryStatus_t BatteryStatusGetStatus();

/**@brief battery status freertos task
 *        is called every 100ms
 */
void BatteryStatusTask();
