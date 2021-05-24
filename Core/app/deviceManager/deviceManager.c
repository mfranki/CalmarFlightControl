/*****************************************************************************
 * @file /CalmarFlightController/Core/app/deviceManager/deviceManager.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date May 24, 2021
 ****************************************************************************/

#include "app/deviceManager/deviceManager.h"

#include "main.h"
#include "cmsis_os.h"

#include "drivers/BMX055/BMX055.h"
#include "drivers/uart/uart.h"
#include "drivers/utils/utils.h"
#include "drivers/radio/radio.h"
#include "drivers/adc/adc.h"
#include "drivers/buzzer/buzzer.h"

#include "middleware/batteryStatus/batteryStatus.h"
#include "middleware/mahonyFilter/mahonyFilter.h"
#include "middleware/soundNotifications/soundNotifications.h"
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
    INIT_LOOP_BMX = 1,
    INIT_LOOP_UART,
    INIT_LOOP_ADC,
    INIT_LOOP_BATTERY_STATUS,
    INIT_LOOP_MAHONY_FILTER
};

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/



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
                       UART_HandleTypeDef* uartDebugHandle)
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
    if(!Bmx055Init(spiBMXHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_BMX)
    }
    if(!UartInit(uartDebugHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_UART)
    }
    if(!AdcInit(adcHandle))
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_ADC)
    }
    /*if(!BatteryStatusInit())
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_BATTERY_STATUS)
    }*/
    if(!MahonyFilterInit())
    {
        INITIALIZATION_FAIL_LOOP(INIT_LOOP_MAHONY_FILTER)
    }


    /** CREATE TASKS **/

    osThreadDef(soundNotificationTask, SoundNotificationTask, osPriorityNormal, 0, 100);
    osThreadCreate(osThread(soundNotificationTask), NULL);

    osThreadDef(radioTask, RadioTask, osPriorityNormal, 0, 100);
    osThreadCreate(osThread(radioTask), NULL);
/*
    osThreadDef(batteryStatusTask, BatteryStatusTask, osPriorityNormal, 0, 100);
    osThreadCreate(osThread(batteryStatusTask), NULL);*/

    osThreadDef(mahonyFilterTask, MahonyFilterTask, osPriorityAboveNormal, 0, 300);
    osThreadCreate(osThread(mahonyFilterTask), NULL);

    osThreadDef(deviceManagerTask, DeviceManagerTask, osPriorityNormal, 0, 100);
    osThreadCreate(osThread(deviceManagerTask), NULL);



}

static void DeviceManagerTask()
{
    while(1)
    {
        osDelay(10);
    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

