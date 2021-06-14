/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/radioStatus/radioStatus.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date May 24, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "drivers/radio/radio.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

#define RADIO_STATUS_MAX_DOWN_TIME_S (1.0f)  ///< [s]


#define RADIO_ROLL_CHANNEL RADIO_CHANNEL_1
#define RADIO_PITCH_CHANNEL RADIO_CHANNEL_2
#define RADIO_THROTTLE_CHANNEL RADIO_CHANNEL_3
#define RADIO_YAW_CHANNEL DARIO_CHANNEL_4
#define RADIO_SWITCH_CHANNEL RADIO_CHANNEL_5
#define RADIO_DIAL_CHANNEL RADIO_CHANNEL_6

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief freertos task
 */
void RadioStatusTask();

/**@brief getter for current radio data
 *
 * @param [in] channel
 * @return channel data, if no radio connection then 0
 */
float RadioStatusGetChannelData(radioChannel_t channel);

/**@brief getter for radio connection status
 *        if radio data is not avalable for RADIO_STATUS_MAX_DOWN_TIME_S
 *        radio status is assumed disconnected
 * @return true if radio connected
 */
bool RadioStatusGetConnectionStatus();
