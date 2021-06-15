/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/memory/memory.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 15, 2021
 ****************************************************************************/

#include "middleware/memory/memory.h"

#include <string.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static void* registeredVariables[EEPROM_VARIABLE_COUNT];

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void MemoryInit()
{
    memset(registeredVariables, 0, sizeof(registeredVariables));
}

bool MemoryRegisterVariable(eepromIndexes_t index, void* address)
{
    if(index >= EEPROM_VARIABLE_COUNT || address == NULL)
    {
        return false;
    }

    registeredVariables[index] = address;

    return true;
}

bool MemorySaveRegisteredVariables()
{
    for(eepromIndexes_t index=0; index<EEPROM_VARIABLE_COUNT; index++)
    {
        if(!EepromWrite(index, registeredVariables[index]))
        {
            return false;
        }
    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

