/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/buzzer/buzzer.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date May 23, 2021
 ****************************************************************************/
#pragma once 

#include <main.h>

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

bool BuzzerInit(TIM_HandleTypeDef* timerHandle, uint32_t timerChannel);



bool BuzzerPlay(uint32_t frequency, uint32_t durationMs);

bool BuzzerActive();

void BuzzerTimerISR();
