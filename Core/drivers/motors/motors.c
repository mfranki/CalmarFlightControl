/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/motors/motors.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date 17 cze 2021
 ****************************************************************************/

#include "drivers/motors/motors.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define CCR_MIN_VALUE (1000U)   ///< equals 1ms
#define CCR_RANGE (1000.0f)     ///< equals 1ms

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

TIM_HandleTypeDef* htim;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool MotorsInit(TIM_HandleTypeDef* hTim)
{
    if(hTim == NULL)
    {
        return false;
    }

    htim = hTim;
    HAL_TIM_Base_Start(htim);
    if(HAL_OK != HAL_TIM_PWM_Start(htim,TIM_CHANNEL_1)) {return false;}
    if(HAL_OK != HAL_TIM_PWM_Start(htim,TIM_CHANNEL_2)) {return false;}
    if(HAL_OK != HAL_TIM_PWM_Start(htim,TIM_CHANNEL_3)) {return false;}
    if(HAL_OK != HAL_TIM_PWM_Start(htim,TIM_CHANNEL_4)) {return false;}

    return true;
}

void MotorsSet(motors_t motor, float power)
{
    if(motor > MOTORS_FRONT_RIGHT)
    {
        return;
    }

    if(power > 1)
    {
        power = 1;
    } else if(power < 0)
    {
        power = 0;
    }
    uint16_t ccrValue = ((uint16_t)(CCR_RANGE*power))+CCR_MIN_VALUE;
    *(&(htim->Instance->CCR1) + motor) = ccrValue;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

