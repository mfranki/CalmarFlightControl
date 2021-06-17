/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/motors/motors.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date 17 cze 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef enum{
    MOTORS_BACK_LEFT = 0,
    MOTORS_FRONT_LEFT,
    MOTORS_BACK_RIGHT,
    MOTORS_FRONT_RIGHT,
}motors_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief starts motor PWM timer
 *
 * @param [in] hTim - itmer handle
 * @return true if successful
 */
bool MotorsInit(TIM_HandleTypeDef* hTim);

/**@brief sets power to motor
 *
 * @param [in] motor
 * @param [in] power - range 0:1 == range 0:100%, values outside are set to maximums
 */
void MotorsSet(motors_t motor, float power);
