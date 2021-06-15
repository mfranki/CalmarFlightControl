/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/eeprom/eeprom.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 15, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef enum{
    EEPROM_ACC_OFFSET_X = 1,
    EEPROM_ACC_OFFSET_Y,
    EEPROM_ACC_OFFSET_Z,
    EEPROM_GYRO_OFFSET_X,
    EEPROM_GYRO_OFFSET_Y,
    EEPROM_GYRO_OFFSET_Z,
    EEPROM_MAG_OFFSET_X,
    EEPROM_MAG_OFFSET_Y,
    EEPROM_MAG_OFFSET_Z,

    EEPROM_VARIABLE_COUNT   ///< max amount of alowed eeprom indexes, not  valid variable
}eepromIndexes_t;


/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes eeprom
 *
 * @return true if successful
 */
bool EepromInit();

/**@brief writes data under given eeprom index
 *
 * @param [in] index - 0::EEPROM_VARIABLE_COUNT-1
 * @param [in] data - 32 bit data pointer
 * @return true if successful
 */
bool EepromWrite(eepromIndexes_t index, void* data);

/**@brief reads data from under given index
 *
 * @param [in] index - 0::EEPROM_VARIABLE_COUNT-1
 * @param [out] data - 32bit data pointer
 * @return true if successful
 */
bool EepromRead(eepromIndexes_t index, void* data);
