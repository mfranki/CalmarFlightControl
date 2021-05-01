/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/adc/adc.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 28, 2021
 ****************************************************************************/

#include "drivers/adc/adc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define ADC_RESOLUTION (4095.0f)  ///< 12 bit
#define ADC_VOLTAGE_DIVIDER_MAX_VOLTAGE (28.4f)    ///< [V] calibrated with multimeter

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static ADC_HandleTypeDef* adcHandle;

static bool waitingForMeasurement = false;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/


/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool AdcInit(ADC_HandleTypeDef* hadc)
{
    if(hadc == NULL)
    {
        return false;
    }
    adcHandle = hadc;

    HAL_ADC_Start(adcHandle);

    return true;
}

float AdcGetBatteryVoltage()
{
    static uint32_t adcRaw = 0;
    waitingForMeasurement = true;
    HAL_ADC_Start_DMA(adcHandle,&adcRaw,1);

    while(waitingForMeasurement){}

    return ((float)adcRaw)*ADC_VOLTAGE_DIVIDER_MAX_VOLTAGE/ADC_RESOLUTION;
}

void AdcDmaIsr()
{
    waitingForMeasurement = false;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/
