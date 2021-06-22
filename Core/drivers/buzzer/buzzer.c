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
#define AUDIO_PWM_PLAY_FREQUENCY (30000U) ///< Hz
#define AUDIO_SAMPLE_QUANTISATION (256U)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static TIM_HandleTypeDef* htim;
static volatile uint32_t timChannel = 0;

static volatile uint32_t cycleCntr = 0;
static volatile uint32_t cycleCount = 0;

static const volatile uint8_t* currentAudioData = NULL;
static volatile uint32_t oversampleCntr = 0;
static volatile uint32_t oversampleCount = 0;

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

bool BuzzerPlayAudio(const uint8_t* audioData, uint32_t audioDataSize, uint32_t sampleRate)
{
    if(audioData == NULL || sampleRate == 0 || AUDIO_PWM_PLAY_FREQUENCY < sampleRate)
    {
        return false;
    }

    currentAudioData = audioData;

    uint32_t prescaler = HAL_RCC_GetSysClockFreq() / (AUDIO_PWM_PLAY_FREQUENCY*AUDIO_SAMPLE_QUANTISATION);
    uint32_t period = AUDIO_SAMPLE_QUANTISATION-1;

    cycleCount = audioDataSize;
    cycleCntr = 0;

    oversampleCount = AUDIO_PWM_PLAY_FREQUENCY/sampleRate;
    oversampleCntr = 0;


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
    if(currentAudioData == NULL)
    {
        cycleCntr++;
        if(cycleCntr > cycleCount)
        {
            HAL_TIM_PWM_Stop(htim, timChannel);
            HAL_TIM_Base_Stop_IT(htim);
        }
    } else {

        oversampleCntr++;
        if(oversampleCntr == oversampleCount)
        {
            oversampleCntr = 0;

            *(timChannel+&(htim->Instance->CCR1)) = currentAudioData[cycleCntr];

            cycleCntr++;
            if(cycleCntr > cycleCount)
            {
                currentAudioData = NULL;
                HAL_TIM_PWM_Stop(htim, timChannel);
                HAL_TIM_Base_Stop_IT(htim);
            }
        }

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
