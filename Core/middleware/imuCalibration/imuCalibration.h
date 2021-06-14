/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/IMU/IMU.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date 1 cze 2021
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

/**@brief freertos task
 */
void ImuCalibrationTask();
