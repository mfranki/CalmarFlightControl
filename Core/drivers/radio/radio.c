/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/radio/radio.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 25, 2021
 ****************************************************************************/

#include "drivers/radio/radio.h"
#include "drivers/utils/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define PWM_MAX_UP_TIME_S (0.002f)  ///< [s], 50Hz PWM -> 2ms == 10% duty cycle
#define PWM_MIN_UP_TIME_S (0.001f)  ///< [s], 50Hz PWM -> 1ms == 5% duty cycle

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static uint32_t channelStateChangeTime[RADIO_CHANNEL_COUNT] = {0,0,0,0,0,0};

/** @brief keeps channel data in range   0..1 **/
static volatile float channelData[RADIO_CHANNEL_COUNT] = {0,0,0,0,0,0};

static volatile bool radioSignalAvailable = false;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void RadioIsr(radioChannel_t channel, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if(channel >= RADIO_CHANNEL_COUNT)
    {
        return;
    }

    float timeElapsed = GetTimeElapsed(&channelStateChangeTime[channel], true);

    if(0 == HAL_GPIO_ReadPin(GPIOx,GPIO_Pin))
    {
        channelData[channel] = (timeElapsed-PWM_MIN_UP_TIME_S)/(PWM_MAX_UP_TIME_S-PWM_MIN_UP_TIME_S);

        if(channelData[channel] > 1){channelData[channel] = 1;}
        if(channelData[channel] < 0){channelData[channel] = 0;}
    }

    radioSignalAvailable = true;
}

radioChannelData_t RadioGetChannelData(radioChannel_t channel)
{
    ASSERT(channel < RADIO_CHANNEL_COUNT)

    radioChannelData_t data = {
            .channelData = channelData[channel],
            .lastUpdateTime = channelStateChangeTime[channel]
    };

    return data;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

