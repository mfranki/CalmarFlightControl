/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/adc/adc.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 28, 2021
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

/**@brief initializes module
 *
 * @param [in] hadc adc instance handle
 * @return true if successful
 */
bool AdcInit(ADC_HandleTypeDef* hadc);

/**@brief reads raw adc data and calculates real battery voltage
 *
 * @return battery voltage in V
 */
float AdcGetBatteryVoltage();

/**@brief tells adc driver that measurement is ready
 *        needs to be implemented in adc dma interrupt service routine
 */
void AdcDmaIsr();


