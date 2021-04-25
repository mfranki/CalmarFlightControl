/*****************************************************************************
 * @file /CALMAR/Core/driver/utils/utils.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date Jun 25, 2020
 ****************************************************************************/

#include "drivers/utils/utils.h"
#include "main.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define Us_IN_S 1000000

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/



/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void UtilsInit()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    ITM->TCR |= 0x01<<3;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

float GetTimeElapsed(uint32_t* lastTimeCalled, bool setCurrentTime)
{
    uint32_t time = DWT->CYCCNT/(SystemCoreClock/Us_IN_S);
    uint32_t timeDiff;
    if(*lastTimeCalled>time)
    {
        timeDiff = UINT_MAX/(SystemCoreClock/Us_IN_S)-*lastTimeCalled+time;
    } else {
        timeDiff = time-*lastTimeCalled;
    }
    if(setCurrentTime)
    {
        *lastTimeCalled = time;
    }

    return ((float)timeDiff)/Us_IN_S;
}

float UtilsMap(float value, float fromMin, float fromMax, float toMin, float toMax)
{
    if(fromMin==fromMax)
    {
        return toMin;
    }
    if(fromMin > fromMax)
    {
        float a = fromMin;
        fromMin = fromMax;
        fromMax = a;
    }

    if(toMin > toMax)
    {
        float a = toMin;
        toMin = toMax;
        toMax = a;
    }

    if(value<fromMin)
    {
        value = fromMin;
    }

    if(value>fromMax)
    {
        value = fromMax;
    }

    return (value-fromMin)/(fromMax-fromMin) * (toMax-toMin) + toMin;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

