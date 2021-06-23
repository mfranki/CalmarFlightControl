/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/altitude/altitude.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 23, 2021
 ****************************************************************************/

#include "drivers/LPS/LPS.h"

#include "middleware/altitude/altitude.h"
#include "middleware/digitalFilter/digitalFilter.h"

#include "cmsis_os.h"
/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define HPA_IN_1M (10.0f)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static digitalFilterHandle_t pressureFilterHandle;

static float homePressure = 1000.0f; ///< [hPa]

static float currentPressure = 1000.0f; ///< [hPa]

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool AltitudeInit()
{
    /** 0.3Hz low pass filter with fs = 25Hz **/
    float numerator[3] = {0.001348711948356, 0.002697423896713, 0.001348711948356};
    float denominator[3] = {1, -1.89346414636182, 0.898858994155252};

    if(!DigitalFilterCreateFilter(numerator, denominator, 2, &pressureFilterHandle))
    {
        return false;
    }
    return true;
}

void AltitudeTask()
{
    homePressure = LPSGetPressure();

    uint32_t previousWakeTime = osKernelSysTick();
    while(1)
    {
        DigitalFilterProcess(pressureFilterHandle, LPSGetPressure(), &currentPressure);

        osDelayUntil(&previousWakeTime,50);
    }
}

float AltitudeGetAltitudeFromHome()
{
    return (homePressure-currentPressure)*HPA_IN_1M;
}

void AltitudeSetHome()
{
    homePressure = currentPressure;
}
/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

