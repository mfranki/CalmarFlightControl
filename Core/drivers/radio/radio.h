/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/radio/radio.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 25, 2021
 ****************************************************************************/
#pragma once 

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
}radioChannel_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

void RadioIrq(uint32_t* data /*radioChannel_t channel, bool risingEdge*/);

void RadioProcess();
