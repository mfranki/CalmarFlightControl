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
    SN_CALIBRATION_FINISHED,

    SN_SETTINGS_MENU_ITEM_1,
    SN_SETTINGS_MENU_ITEM_5,
}SoundNotifications_t;

typedef enum{
    SN_ZERO,
    SN_ONE,
    SN_TWO,
    SN_THREE,
    SN_FOUR,
    SN_FIVE,
    SN_SIX,
    SN_SEVEN,
    SN_EIGHT,
    SN_NINE,
    SN_DOT,
    SN_MINUS
}SoundNotificationsAudioFiles_t;

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

/**@brief plays selected audio in non blocking mode
 *
 * @param [in] audioFile @ref audioData.h
 */
void SoundNotificationsPlayAudio(SoundNotificationsAudioFiles_t audioFile);
