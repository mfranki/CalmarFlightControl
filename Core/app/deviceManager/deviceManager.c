/*****************************************************************************
 * @file /CalmarFlightController/Core/app/deviceManager/deviceManager.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date May 24, 2021
 ****************************************************************************/

#include <middleware/imuCalibration/imuCalibration.h>
#include "app/deviceManager/deviceManager.h"

#include "main.h"
#include "cmsis_os.h"

#include "drivers/BMX055/BMX055.h"
#include "drivers/uart/uart.h"
#include "drivers/utils/utils.h"
#include "drivers/radio/radio.h"
#include "drivers/adc/adc.h"
#include "drivers/buzzer/buzzer.h"
#include "drivers/eeprom/eeprom.h"
#include "drivers/motors/motors.h"

#include "middleware/batteryStatus/batteryStatus.h"
#include "middleware/mahonyFilter/mahonyFilter.h"
#include "middleware/soundNotifications/soundNotifications.h"
#include "middleware/radioStatus/radioStatus.h"
#include "middleware/remoteSettings/remoteSettings.h"
#include "middleware/memory/memory.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

/** rings buzzer according to ERROR_INDEX **/
#define INITIALIZATION_FAIL_LOOP(ERROR_INDEX) while(1)                                                                          \
                                              {                                                                                 \
                                                  SoundNotificationsPlayInBlockingMode(SN_INITIALIZATION_ERROR);                \
                                                  uint8_t count = ERROR_INDEX<1 ? 0 : ERROR_INDEX-1;                            \
                                                  for(uint8_t i=0; i<count; i++)                                                \
                                                  {                                                                             \
                                                      SoundNotificationsPlayInBlockingMode(SN_INITIALIZATION_ERROR_INDEX);      \
                                                  }                                                                             \
                                              }

enum{
    INIT_LOOP_UART = 1,
    INIT_LOOP_EEPROM,
    INIT_LOOP_BMX,
    INIT_LOOP_ADC,
    INIT_LOOP_MOTORS
};



#define THROTTLE_OFF_TRH (0.05f)
#define SWITCH_OFF_TRH (0.25f)
#define FLIGHT_MODE_ZEROS_MAX_COUNT (10U)
#define HOMING_COUNTER_MAX_COUNT (10U)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static deviceOperatingModes_t operatingMode = DEVICE_INITIALIZATION;

static struct{
    TaskHandle_t soundNotificationTask;
    TaskHandle_t radioStatusTask;
    TaskHandle_t remoteSettingsTask;
    TaskHandle_t batteryStatusTask;
    TaskHandle_t mahonyFilterTask;
    TaskHandle_t imuCalibrationTask;
    TaskHandle_t deviceManagerTask;
}taskHandles;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief device manager task
 *
 */
static void DeviceManagerTask();

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void DeviceManagerInit(ADC_HandleTypeDef* adcHandle,
                       SPI_HandleTypeDef* spiBMXHandle,
                       TIM_HandleTypeDef* timBuzzerHandle,
                       UART_HandleTypeDef* uartDebugHandle,
                       TIM_HandleTypeDef* timMotorsHandle)
{
    if(adcHandle == NULL ||
       spiBMXHandle == NULL ||
       timBuzzerHandle == NULL ||
       uartDebugHandle == NULL)
    {
        while(1){}
    }

    UtilsInit();
    if(!BuzzerInit(timBuzzerHandle, TIM_CHANNEL_1))
    {
        while(1){}
    }
    if(!UartInit(uartDebugHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_UART)
    }
    if(!EepromInit())
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_EEPROM)
    }
    MemoryInit();
    if(!Bmx055Init(spiBMXHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_BMX)
    }
    if(!AdcInit(adcHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_ADC)
    }
    if(!MotorsInit(timMotorsHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_MOTORS)
    }

    /** CREATE TASKS **/

    xTaskCreate(&SoundNotificationTask, "soundNotificationTask", 100,  NULL, 0, &(taskHandles.soundNotificationTask));
    xTaskCreate(&RadioStatusTask,       "radioStatusTask",       100,  NULL, 0, &(taskHandles.radioStatusTask      ));
    xTaskCreate(&RemoteSettingsTask,    "remoteSettingsTask",    100,  NULL, 0, &(taskHandles.remoteSettingsTask   ));
    xTaskCreate(&BatteryStatusTask,     "batteryStatusTask",     100,  NULL, 0, &(taskHandles.batteryStatusTask    ));
    xTaskCreate(&MahonyFilterTask,      "mahonyFilterTask",      300,  NULL, 1, &(taskHandles.mahonyFilterTask     ));
    xTaskCreate(&ImuCalibrationTask,    "imuCalibrationTask",    1000, NULL, 0, &(taskHandles.imuCalibrationTask              ));
    xTaskCreate(&DeviceManagerTask,     "deviceManagerTask",     200,  NULL, 0, &(taskHandles.deviceManagerTask    ));

    operatingMode = DEVICE_STANDBY;

}

deviceOperatingModes_t DeviceManagerGetOperatingMode()
{
    return operatingMode;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static void DeviceManagerTask()
{
    while(1)
    {
        /** STANDBY MODE **/
        if(operatingMode == DEVICE_STANDBY)
        {
            if(BatteryStatusGetStatus() != BATTERY_OK)
            {
                RemoteSettingsSetVariable(RS_CALIBRATION, 0.0f);
                operatingMode = DEVICE_ERROR;
                continue;
            }

            if(RadioStatusGetChannelData(RADIO_THROTTLE_CHANNEL) > THROTTLE_OFF_TRH)
            {
                operatingMode = DEVICE_FLIGHT;
                continue;
            }

            if(RadioStatusGetChannelData(RADIO_SWITCH_CHANNEL) > SWITCH_OFF_TRH)
            {
                operatingMode = DEVICE_SETTINGS;
                continue;
            }
        }

        /** SETTINGS_MODE **/
        if(operatingMode == DEVICE_SETTINGS)
        {
            float calibration = 0;
            RemoteSettingsGetVariable(RS_CALIBRATION, &calibration);
            if(RadioStatusGetChannelData(RADIO_SWITCH_CHANNEL) < SWITCH_OFF_TRH)
            {
                if(calibration<-1 || calibration>1)
                {
                    operatingMode = DEVICE_CALIBRATION;
                    vTaskResume(taskHandles.imuCalibrationTask);

                    continue;
                } else  {
                    operatingMode = DEVICE_STANDBY;
                    MemorySaveRegisteredVariables();
                    continue;
                }

            }

            if(BatteryStatusGetStatus() != BATTERY_OK)
            {
                RemoteSettingsSetVariable(RS_CALIBRATION, 0.0f);
                operatingMode = DEVICE_ERROR;
                continue;
            }
        }

        /** CALIBRATION MODE **/
        if(operatingMode == DEVICE_CALIBRATION)
        {
            if(eSuspended == eTaskGetState(taskHandles.imuCalibrationTask))
            {
                RemoteSettingsSetVariable(RS_CALIBRATION, 0.0f);
                operatingMode = DEVICE_STANDBY;
                MemorySaveRegisteredVariables();
                continue;
            }

            if(RadioStatusGetChannelData(RADIO_SWITCH_CHANNEL) > SWITCH_OFF_TRH)
            {
                RemoteSettingsSetVariable(RS_CALIBRATION, 0.0f);
                operatingMode = DEVICE_SETTINGS;
                continue;
            }

            if(BatteryStatusGetStatus() != BATTERY_OK)
            {
                RemoteSettingsSetVariable(RS_CALIBRATION, 0.0f);
                operatingMode = DEVICE_ERROR;
                continue;
            }
        }

        /** FLIGHT MODE **/
        if(operatingMode == DEVICE_FLIGHT)
        {
            static uint16_t flightModeZerosCounter = 0;

            if(RadioStatusGetChannelData(RADIO_THROTTLE_CHANNEL) < THROTTLE_OFF_TRH)
            {
                if(flightModeZerosCounter++ >= FLIGHT_MODE_ZEROS_MAX_COUNT)
                {
                    flightModeZerosCounter = 0;
                    operatingMode = DEVICE_STANDBY;
                    continue;
                }
            } else {
                flightModeZerosCounter = 0;
            }

            if((!RadioStatusGetConnectionStatus()) || (BatteryStatusGetStatus() != BATTERY_OK))
            {
                flightModeZerosCounter = 0;
                operatingMode = DEVICE_HOMING;
                continue;
            }

        }

        /**HOMING MODE **/
        if(operatingMode == DEVICE_HOMING)
        {
            static uint16_t homingCounter = 0;

            if(0)       ///< check if device reached home position
            {
                homingCounter = 0;
                operatingMode = DEVICE_STANDBY;
                continue;
            }

            if(RadioStatusGetConnectionStatus())
            {
                if(homingCounter++ >= HOMING_COUNTER_MAX_COUNT)
                {
                    homingCounter = 0;
                    operatingMode = DEVICE_FLIGHT;
                    continue;
                }
            } else {
                homingCounter = 0;
            }
        }



    /*    quaternion_t q = MahonyFilterGetOrientation();
        vector_t v = QuatTranslateToRotationVector(q);
        UartWrite("%f\t%f\t%f\r\n",v.x*180/3.141,v.y*180/3.141,v.z*180/3.141);
        //UartWrite("%f\t%f\t%f\t%f\r\n",q.w,q.i,q.j,q.k);
/*
        bmx055Data_t imuData;
        Bmx055GetData(&imuData);
/*        UartWrite("%f\t %f\t %f\t %f\t %f\t %f\t \r\n",imuData.ax,
                                                       imuData.ay,
                                                       imuData.az,
                                                       imuData.gx,
                                                       imuData.gy,
                                                       imuData.gz);*/
    /*
        static const char* modes[] = {
                "DEVICE_INITIALIZATION\n",  /// 0
                "DEVICE_STANDBY\n",         /// 1
                "DEVICE_CALIBRATION\n",     /// 2
                "DEVICE_SETTINGS\n",        /// 3
                "DEVICE_FLIGHT\n",          /// 4
                "DEVICE_HOMING\n",          /// 5
                "DEVICE_ERROR\n"
        };

        UartWrite(modes[operatingMode]);*/

        osDelay(100);
    }
}

