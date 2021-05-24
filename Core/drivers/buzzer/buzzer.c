/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/buzzer/buzzer.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date May 23, 2021
 ****************************************************************************/

#include "drivers/buzzer/buzzer.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define TIMER_BASE_FREQUENCY (1000000U) ///< Hz

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static TIM_HandleTypeDef* htim;
static uint32_t timChannel = 0;

static uint32_t cycleCntr = 0;
static uint32_t cycleCount = 0;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

static bool SetupTimer(uint32_t prescaler, uint32_t period);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool BuzzerInit(TIM_HandleTypeDef* timerHandle, uint32_t timerChannel)
{
	if(timerHandle == NULL ||
	   (timerChannel != TIM_CHANNEL_1 &&
	    timerChannel != TIM_CHANNEL_2 &&
	    timerChannel != TIM_CHANNEL_3 &&
	    timerChannel != TIM_CHANNEL_4))
	{
		return false;
	}

	htim = timerHandle;
	timChannel = timerChannel;

	return true;
}

bool BuzzerPlay(uint32_t frequency, uint32_t durationMs)
{
	if(frequency == 0 || durationMs == 0)
	{
		return false;
	}

	uint32_t prescaler = HAL_RCC_GetSysClockFreq() / TIMER_BASE_FREQUENCY;
	uint32_t period = TIMER_BASE_FREQUENCY / frequency;
	cycleCount = (uint32_t)((((uint64_t)durationMs)*1000)/((uint64_t)period));
	cycleCntr = 0;


	if(!SetupTimer(prescaler, period))
	{
		return false;
	}

	return true;
}

bool BuzzerActive()
{
    return cycleCntr <= cycleCount;
}

void BuzzerTimerISR()
{
    cycleCntr++;
    if(cycleCntr > cycleCount)
    {
        HAL_TIM_PWM_Stop(htim, timChannel);
        HAL_TIM_Base_Stop_IT(htim);
    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static bool SetupTimer(uint32_t prescaler, uint32_t period)
{
	htim->Init.Prescaler = prescaler;
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = period;
	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(htim) != HAL_OK)
	{
		return false;
	}
	if (HAL_TIM_PWM_Init(htim) != HAL_OK)
	{
		return false;
	}

	TIM_OC_InitTypeDef sConfigOC;
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = period/2;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timChannel) != HAL_OK)
	{
		return false;
	}

	HAL_TIM_PWM_Start(htim, timChannel);
	HAL_TIM_Base_Start_IT(htim);

	HAL_TIM_MspPostInit(htim);

	return true;
}
