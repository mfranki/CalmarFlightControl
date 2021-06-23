/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/LPS/LPS.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 22, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes sensor
 *
 * @param [in] HSPI - spi handle
 * @return true if successful
 */
bool LPSInit(SPI_HandleTypeDef *HSPI);

/**@brief gets pressure readings in [hPa] from LPS sensor
 *        max read frequency 25Hz
 *
 * @return pressure reading [hPa]
 */
float LPSGetPressure();
