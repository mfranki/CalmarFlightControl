/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/eeprom/eeprom.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 15, 2021
 ****************************************************************************/

#include "drivers/eeprom/eeprom.h"

#include "main.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define EEPROM_FLASH_SECTOR FLASH_SECTOR_2
#define EEPROM_BEGIN (0x8008000U)   ///< first eeprom address
#define EEPROM_END (0x800BFF8U)     ///< last eeprom address

#define EEPROM_SIZE (EEPROM_END+8-EEPROM_BEGIN)     ///< has to be power of 2

#define EMPTY32 (0xFFFFFFFFU)   ///< value of 32bit empty index cell
#define CELL_SIZE (8U)          ///< bytes



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

/**@brief keeps track on first writable memory location
 */
uint32_t lastVariableOffset = 0;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief reads 32 bit data from specified addres
 *
 * @param [in] offset - offset from EEPROM_BEGIN
 * @param [out] data - 32 bit data pointer
 * @return true if successful
 */
bool ReadMemoryLocation(uint32_t offset, void* data);

/**@brief writes data to cell under specified address
 *
 * @param [in] offset - offset from EEPROM_BEGIN, needs to be multiple of CELL_SIZE
 * @param [in] index - virtual address of cell, variable index od @param data
 * @param [in] data - 32 bit data pointer
 * @return
 */
bool WriteData(uint32_t offset, uint32_t index, void* data);

/**@brief finds last non EMPTY address in EEPROM sector
 *        uses binary search
 *
 * @return address as offset from EEPROM_BEGIN
 */
uint32_t FindLastVariableAddress();

/**@brief stores all newest eeprom variables in ram array
 *        clears eeprom sector and writes variables to eeprom
 */
void RefreshEeprom();

/**@brief erases FLASH sector: EEPROM_FLASH_SECTOR
 */
void EraseEeprom();
/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool EepromInit()
{
    if(HAL_OK != HAL_FLASH_Unlock())
    {
        return false;
    }
    lastVariableOffset = FindLastVariableAddress();

    return true;
}

bool EepromWrite(eepromIndexes_t index, void* data)
{
    if(index >= EEPROM_VARIABLE_COUNT)
    {
        return false;
    }

    if(!WriteData(lastVariableOffset, index, data))
    {
        return false;
    }

    if((lastVariableOffset+=CELL_SIZE) >= EEPROM_SIZE)
    {
        RefreshEeprom();
    }

    return true;
}

bool EepromRead(eepromIndexes_t index, void* data)
{
    for(int32_t offset = lastVariableOffset; offset>=0; offset -= CELL_SIZE)
    {
        uint32_t ind;
        ReadMemoryLocation(offset,&ind);

        if(ind == index)
        {
            return ReadMemoryLocation(offset+CELL_SIZE/2,data);
        }
    }

    return false;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

bool ReadMemoryLocation(uint32_t offset, void* data)
{
    if(offset > EEPROM_SIZE-CELL_SIZE/2)
    {
        return false;
    }

    memcpy(data, (void*)(EEPROM_BEGIN+offset), sizeof(uint32_t));
    
    return true;
}

bool WriteData(uint32_t offset, uint32_t index, void* data)
{
    while((FLASH->SR&FLASH_SR_BSY) != 0){}

    FLASH->CR = 0x00000000;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;

    *(uint32_t*)(EEPROM_BEGIN+offset) = (uint32_t)index;
    *(uint32_t*)(EEPROM_BEGIN+offset+4) = *(uint32_t*)data;

    if(__HAL_FLASH_GET_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                               FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR)) != RESET)
    {
        return false;
    }

    return true;
}

uint32_t FindLastVariableAddress()
{
    uint32_t offset = EEPROM_SIZE/2;
    uint32_t step = EEPROM_SIZE/2;
    
    while(step >= CELL_SIZE)
    {
        step = step/2;
        uint32_t data;
        ReadMemoryLocation(offset, &data);
        
        if(data == EMPTY32)
        {
            ReadMemoryLocation(offset-CELL_SIZE, &data);
            if(data != EMPTY32)
            { 
                return offset;
            }

            offset -= step;
        } else {
            if(offset == EEPROM_SIZE-CELL_SIZE)
            {
                RefreshEeprom();
                return 0;
            }
            
            ReadMemoryLocation(offset+CELL_SIZE, &data);
            if(data == EMPTY32)
            {
                return offset+CELL_SIZE;
            }

            offset += step;
        }
    }

    return 0;
}

void RefreshEeprom()
{
    uint32_t  variables[EEPROM_VARIABLE_COUNT];
    bool variablesDone[EEPROM_VARIABLE_COUNT];
    uint32_t variablesDoneSum = 0;

    memset(variables,0,EEPROM_VARIABLE_COUNT*sizeof(uint32_t));
    memset(variablesDone,0,EEPROM_VARIABLE_COUNT*sizeof(bool));

    for(int32_t offset = EEPROM_SIZE-CELL_SIZE;
            offset>=0 && variablesDoneSum<EEPROM_VARIABLE_COUNT;
                offset -= CELL_SIZE)
    {
        uint32_t index;
        ReadMemoryLocation(offset, &index);
        if((index < EEPROM_VARIABLE_COUNT) && (variablesDone[index] == false))
        {
            ReadMemoryLocation(offset+CELL_SIZE/2, (&variables[index]));

            variablesDone[index] = true;
            variablesDoneSum++;
        }
    }

    EraseEeprom();

    lastVariableOffset = 0;

    for(uint32_t index=0; index<EEPROM_VARIABLE_COUNT; index++)
    {
        if(variablesDone[index])
        {
            EepromWrite(index, (&variables[index]));
        }
    }
}

void EraseEeprom()
{
    while((FLASH->SR&FLASH_SR_BSY) != 0){}

    FLASH->CR = 0x00000000;
    FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
    FLASH->CR |= EEPROM_FLASH_SECTOR<<FLASH_CR_SNB_Pos;
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= FLASH_CR_STRT;

    while((FLASH->SR&FLASH_SR_BSY) != 0){}
}
