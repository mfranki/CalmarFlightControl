/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/flightController/flightController.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 18, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes PID regulators and filters
 *
 * @return true if successful
 */
bool FlightControllerInit();

/**@brief freertos task
 */
void FlightControllerTask();
