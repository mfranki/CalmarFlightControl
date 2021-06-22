/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/remoteSettings/remoteSettings.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 6, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

/**@warning when adding new settings variable, it is required to update:
 *  - eeprom saving mechanism in RemoteSettingsInit
 *  - variablesDefaultValues
 *  - variablesMultipliers
 */
typedef enum{
    RS_CALIBRATION = 0,/**< RS_CALIBRATION */
    RS_PID_XY_P,       /**< RS_PID_XY_P */
    RS_PID_XY_I,       /**< RS_PID_XY_I */
    RS_PID_XY_D,       /**< RS_PID_XY_D */
    RS_PID_Z_P,        /**< RS_PID_Z_P */
    RS_PID_Z_I,        /**< RS_PID_Z_I */
    RS_PID_Z_D,        /**< RS_PID_Z_D */
    RS_PID_N,          /**< RS_PID_N */

    RS_VARIABLES_COUNT /**< RS_VARIABLES_COUNT */
}settingsVariable_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief registers settings variables in memory
 *
 * @return true if successful
 */
bool RemoteSettingsInit();

/**@brief freertos task
 */
void RemoteSettingsTask();

/**@brief adds callback to listy of callbacks, when RADIO_SWITCH_CHANNEL goes back to 0
 *        whole callback list is called
 *
 * @param [in] updateCallback
 * @return true if successful
 */
bool RemoteSettingsAddUpdateCallback(void (*updateCallback)(void));

/**@brief sets settings variable
 *
 * @param [in] variable
 * @param [in] value
 * @return true if successful
 */
bool RemoteSettingsSetVariable(settingsVariable_t variable, float value);

/**@brief getter for settings variable
 *
 * @param [in] variable
 * @param [out] value
 * @return true if successful
 */
bool RemoteSettingsGetVariable(settingsVariable_t variable, float *value);
