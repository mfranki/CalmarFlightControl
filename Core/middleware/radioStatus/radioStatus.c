/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/radioStatus/radioStatus.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date May 24, 2021
 ****************************************************************************/

#include "middleware/radioStatus/radioStatus.h"
#include "drivers/utils/utils.h"

#include "cmsis_os.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static float radioChannelCurrentData[RADIO_CHANNEL_COUNT] = {0,0,0,0,0,0};
static bool radioSignalAvailable = false;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void RadioStatusTask()
{
    while(1)
    {
        bool allRadioChannelsAvailable = true;
        for(radioChannel_t channel=RADIO_CHANNEL_1; channel<RADIO_CHANNEL_COUNT; channel++)
        {
            radioChannelData_t data = RadioGetChannelData(channel);
            if(GetTimeElapsed(&(data.lastUpdateTime), false) > RADIO_STATUS_MAX_DOWN_TIME_S)
            {
                radioChannelCurrentData[channel] = 0;
                allRadioChannelsAvailable = false;
            } else {
                radioChannelCurrentData[channel] = data.channelData;
            }
        }

        radioSignalAvailable = allRadioChannelsAvailable;

        osDelay(20);
    }
}

float RadioStatusGetChannelData(radioChannel_t channel)
{
    ASSERT(channel < RADIO_CHANNEL_COUNT)

    return radioChannelCurrentData[channel];
}

bool RadioStatusGetConnectionStatus()
{
    return radioSignalAvailable;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

