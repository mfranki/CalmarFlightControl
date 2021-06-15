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

/**@brief initializes registeredVariables array
 */
void MemoryInit();

/**@brief adds variable address to registered variables
 *
 * @param [in] index - variable index
 * @param [in] address - address of 32bit variable
 * @return true if successful
 */
bool MemoryRegisterVariable(eepromIndexes_t index, void* address);

/**@brief takes data from all registered variables and stores it in EEPROM
 *
 * @return true if successful
 */
bool MemorySaveRegisteredVariables();
