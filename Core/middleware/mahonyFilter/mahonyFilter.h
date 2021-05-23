/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/mahonyFilter/mahonyFilter.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
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

bool MahonyFilterInit();

void MahonyFilterTask();
