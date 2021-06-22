/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/batteryStatus/batteryStatus.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/

#include "drivers/adc/adc.h"

#include "middleware/batteryStatus/batteryStatus.h"
#include "middleware/soundNotifications/soundNotifications.h"

#include "cmsis_os.h"

#include <main.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define CELL_VOLTAGE_MIN (3.0f) ///< [V]
#define CELL_VOLTAGE_MIN_SAFE (3.2f) ///< [V]
#define CELL_VOLTAGE_DEPLEATED (3.3f) ///< [V]
#define CELL_VOLTAGE_LOW (3.5f)  ///< [V]
#define CELL_VOLTAGE_MAX_SAFE (4.3f) ///< [V]
#define CELL_VOLTAGE_MAX  (4.4f) ///< [V]

#define CELL_VOLTAGE_HYSTERESIS (0.05f) ///< [V]

#define MAX_CELL_COUNT (5U)
#define BATTERY_MEAS_CELL_COUNT_RETRIES (5U)

#define MEASUREMENTS_COUNT (10U)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static uint8_t detectedCellCount = 0;

static batteryStatus_t batteryStatus = BATTERY_OK;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief detects cell count in battery based on battery voltage
 *
 * @return if successful cell count, if not, 0
 *         (this means battery voltage is incorrect: over charged/over discharged cells)
 */
static uint8_t DetectCellCount();


/**@brief reads battery voltage and determines its status based on ranges
 *
 * @param [in] voltage - measured battery voltage
 * @param [out] hysteresisRange - if true battery is in given status hysteresis range
 * @return battery status based on given voltage
 */
static batteryStatus_t GetMomentaryBatteryStatus(float voltage, bool* hysteresisRange);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

batteryStatus_t BatteryStatusGetStatus()
{
    return batteryStatus;
}

void BatteryStatusTask()
{
    for(uint8_t tries=0; detectedCellCount==0 && tries<BATTERY_MEAS_CELL_COUNT_RETRIES ; tries++)
    {
        detectedCellCount = DetectCellCount();
    }

    while(detectedCellCount == 0)
    {
        batteryStatus = BATTERY_ERROR;
        SoundNotificationsPlay(SN_BATTERY_ERROR);
        osDelay(1000);
    }

    bool hr = 0;
    batteryStatus = GetMomentaryBatteryStatus(AdcGetBatteryVoltage(),&hr);

    uint8_t measurementCntr = 0;
    float measurementSum = 0;
    while(1)
    {
        if(batteryStatus == BATTERY_LOW)
        {
            SoundNotificationsPlay(SN_BATTERY_LOW);
        }else if(batteryStatus != BATTERY_OK)
        {
            SoundNotificationsPlay(SN_BATTERY_ERROR);
        }

        osDelay(1000);

        measurementSum += AdcGetBatteryVoltage();

        measurementCntr++;
        if(measurementCntr < MEASUREMENTS_COUNT)
        {
            continue;
        }

        bool hysteresisRegion = false;
        batteryStatus_t tempBatteryStatus = GetMomentaryBatteryStatus(measurementSum/(float)MEASUREMENTS_COUNT,&hysteresisRegion);

        measurementSum = 0;
        measurementCntr = 0;

        if(batteryStatus == tempBatteryStatus)
        {
            continue;
        }

        if(abs(((int8_t)batteryStatus)-((int8_t)tempBatteryStatus)) > 1)
        {
            batteryStatus = tempBatteryStatus;
            continue;
        }

        if(hysteresisRegion)
        {
            continue;
        }

        batteryStatus = tempBatteryStatus;
    }
}
/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static uint8_t DetectCellCount()
{
    float voltage = AdcGetBatteryVoltage();

    for(uint8_t cellCount=1; cellCount<=MAX_CELL_COUNT; cellCount++)
    {
         if(voltage >= CELL_VOLTAGE_MIN*((float)cellCount) &&
            voltage <= CELL_VOLTAGE_MAX*((float)cellCount))
         {
             return cellCount;
         }
    }
    return 0;
}

static batteryStatus_t GetMomentaryBatteryStatus(float voltage, bool* hysteresisRange)
{
    /** UNDERVOLTAGE **/
    if(voltage < CELL_VOLTAGE_MIN_SAFE*((float)detectedCellCount))
    {
        *hysteresisRange = false;
        return BATTERY_UNDERVOLTAGE;
    }

    /** DEPLEATED VOLTAGE **/
    if(voltage >= CELL_VOLTAGE_MIN_SAFE*((float)detectedCellCount) &&
       voltage < (CELL_VOLTAGE_MIN_SAFE+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount))
    {
        *hysteresisRange = true;
        return BATTERY_DEPLEATED;
    }

    if(voltage >= (CELL_VOLTAGE_MIN_SAFE+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount) &&
       voltage < CELL_VOLTAGE_DEPLEATED*((float)detectedCellCount))
    {
        *hysteresisRange = false;
        return BATTERY_DEPLEATED;
    }

    /** LOW VOLTAGE **/
    if(voltage >= CELL_VOLTAGE_DEPLEATED*((float)detectedCellCount) &&
       voltage < (CELL_VOLTAGE_DEPLEATED+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount))
    {
        *hysteresisRange = true;
        return BATTERY_LOW;
    }

    if(voltage >= (CELL_VOLTAGE_DEPLEATED+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount) &&
       voltage < CELL_VOLTAGE_LOW*((float)detectedCellCount))
    {
        *hysteresisRange = false;
        return BATTERY_LOW;
    }

    /** OK VOLTAGE **/

    if(voltage >= CELL_VOLTAGE_LOW*((float)detectedCellCount) &&
       voltage < (CELL_VOLTAGE_LOW+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount))
    {
        *hysteresisRange = true;
        return BATTERY_OK;
    }

    if(voltage >= (CELL_VOLTAGE_LOW+CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount) &&
       voltage < (CELL_VOLTAGE_MAX_SAFE-CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount))
    {
        *hysteresisRange = false;
        return BATTERY_OK;
    }

    if(voltage >= (CELL_VOLTAGE_MAX_SAFE-CELL_VOLTAGE_HYSTERESIS)*((float)detectedCellCount) &&
       voltage <= CELL_VOLTAGE_MAX_SAFE*((float)detectedCellCount))
    {
        *hysteresisRange = true;
        return BATTERY_OK;
    }

    /** OVERVOLTAGE **/

    *hysteresisRange = false;
    return BATTERY_OVERVOLTAGE;
}
