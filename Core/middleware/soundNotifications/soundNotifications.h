/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/soundNotifications/soundNotifications.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date May 23, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef enum{
    SN_ASSERT,

    SN_INITIALIZATION_ERROR,
    SN_INITIALIZATION_ERROR_INDEX,

    SN_BATTERY_LOW,
    SN_BATTERY_DEPLEATED,

    SN_CALIBRATION_START,
    SN_MAGNETOMETER_CALIBRATION_AXIS_DONE,
    SN_MAGNETOMETER_CALIBRATION_INVALID_AXIS,
    SN_CALIBRATION_FINISHED
}SoundNotifications_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief sound notification freertos task
 *        used to play notifications in non blocking mode
 */
void SoundNotificationTask();

/**@brief Sends data to SoundNotificationTask that plays it in non blocking mode
 *         If the notification is still playing, playing new notification is blocked
 *
 * @param [in] notification
 * @return true if sending notification succeded, false if notification is still playing
 */
bool SoundNotificationsPlay(SoundNotifications_t notification);

/**@brief Plays notification in blocking mode - waits in loop until sound stops playing
 *
 * @param [in] notification
 */
void SoundNotificationsPlayInBlockingMode(SoundNotifications_t notification);
