/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/radio/radio.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 25, 2021
 ****************************************************************************/
#pragma once 

#include <main.h>
#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef enum{
    RADIO_CHANNEL_1,
    RADIO_CHANNEL_2,
    RADIO_CHANNEL_3,
    RADIO_CHANNEL_4,
    RADIO_CHANNEL_5,
    RADIO_CHANNEL_6,
    RADIO_CHANNEL_COUNT
}radioChannel_t;

typedef struct{
    float channelData;
    uint32_t lastUpdateTime;
}radioChannelData_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief needs to be called when rising or falling interrupt on each radio channel arrives
 *        interrupt needs to be cleared only when this function for each channel is executed
 *
 * @param [in] channel
 * @param [in] GPIOx - GPIO port
 * @param [in] GPIO_Pin
 */
void RadioIsr(radioChannel_t channel, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**@brief getter for radio channel data and last update time
 *        asserts if channel is invalid
 *
 * @param [in] channel
 * @return radio channel data and last update time
 */
radioChannelData_t RadioGetChannelData(radioChannel_t channel);

