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
