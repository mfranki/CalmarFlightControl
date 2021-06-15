/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/memory/memory.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 15, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "drivers/eeprom/eeprom.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

void MemoryInit();

bool MemoryRegisterVariable(eepromIndexes_t index, void* address);

bool MemorySaveRegisteredVariables();
