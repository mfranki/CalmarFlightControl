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

/**@brief initializes buzzer
 *
 * @param [in] timerHandle
 * @param [in] timerChannel
 * @return true if successful
 */
bool BuzzerInit(TIM_HandleTypeDef* timerHandle, uint32_t timerChannel);

/**@brief plays tone in non blocking mode
 *
 * @param [in] frequency
 * @param [in] durationMs
 * @return true if starting tune was successful
 */
bool BuzzerPlay(uint32_t frequency, uint32_t durationMs);

/**@brief plays audio from array
 *
 * @param [in] audioData
 * @param [in] audioDataSize
 * @param [in] sampleRate
 * @return true if starting playing was successful
 */
bool BuzzerPlayAudio(const uint8_t* audioData, uint32_t audioDataSize, uint32_t sampleRate);

/**@brief checker for buzzer activity
 *
 * @return true if sound is playing
 */
bool BuzzerActive();

/**@brief buzzer ISR
 *        needs to be placed before clearing interrupt
 */
void BuzzerTimerISR();
