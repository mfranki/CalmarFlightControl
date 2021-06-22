/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/soundNotifications/soundNotifications.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date May 23, 2021
 ****************************************************************************/

#include <middleware/soundNotifications/audioData8000.h>
#include "middleware/soundNotifications/soundNotifications.h"

#include "drivers/buzzer/buzzer.h"
#include "cmsis_os.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define NOTIFICATION_SIZE (10U) ///< max amount of tones in 1 notification

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef struct{
    uint32_t frequency;
    uint32_t duration;
}soundSample_t;

typedef struct{
    uint8_t size;
    soundSample_t samples[NOTIFICATION_SIZE];
}soundData_t;

/** notifications tones **/
static const soundData_t notifications[] = {
    {                           ///<     SN_ASSERT
        .size = 2,
        .samples = {{3000 , 500},
                    {40000, 500}}
    },
    {                           ///< SN_INITIALIZATION_ERROR,
        .size = 2,
        .samples = {{2000 , 200},
                    {40000, 200}}
    },
    {                           ///< SN_INITIALIZATION_ERROR_INDEX
        .size = 2,
        .samples = {{1000 , 200},
                    {40000, 200}}
    },
    {                           ///< SN_BATTERY_LOW
        .size = 5,
        .samples = {{500  , 50 },
                    {1000 , 100},
                    {40000, 100},
                    {500  , 50 },
                    {1000 , 100}}
    },
    {                           ///< SN_BATTERY_DEPLEATED
        .size = 5,
        .samples = {{1000 , 100},
                    {500  , 100},
                    {200  , 300}}

    },
    {                           ///< SN_CALIBRATION_START
        .size = 3,
        .samples = {{500 , 100},
                    {700 , 100},
                    {900 , 100},}

    },
    {                           ///< SN_MAGNETOMETER_CALIBRATION_AXIS_DONE
        .size = 4,
        .samples = {{900   , 100},
                    {40000 , 50},
                    {900   , 100},
                    {40000 , 50}}

    },
    {                           ///< SN_MAGNETOMETER_CALIBRATION_INVALID_AXIS
        .size = 6,
        .samples = {{900   , 100},
                    {40000 , 50},
                    {500   , 100},
                    {40000 , 50},
                    {200   , 100},
                    {40000 , 50}}

    },
    {                           ///< SN_CALIBRATION_FINISHED
        .size = 10,
        .samples = {{495   , 100},
                    {40000 , 50},
                    {661   , 100},
                    {40000 , 50},
                    {986   , 100},
                    {40000 , 50},
                    {986   , 100},
                    {40000 , 50}}

    },
    {                           ///< SN_SETTINGS_MENU_ITEM_1
        .size = 1,
        .samples = {{1300  , 100}}

    },
    {                           ///< SN_SETTINGS_MENU_ITEM_5
        .size = 1,
        .samples = {{1300  , 400}}

    },
};

QueueHandle_t soundQueueHandle = NULL; ///< holds currently playing notification

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void SoundNotificationTask()
{
    soundQueueHandle = xQueueCreate(NOTIFICATION_SIZE,sizeof( soundSample_t ) );

    while(1)
    {
        soundSample_t currentSample;
        if(pdTRUE != xQueueReceive(soundQueueHandle,&currentSample,portMAX_DELAY))
        {
            continue;
        }

        BuzzerPlay(currentSample.frequency, currentSample.duration);
        vTaskDelay(currentSample.duration);

    }
}

bool SoundNotificationsPlay(SoundNotifications_t notification)
{
    if(uxQueueMessagesWaiting(soundQueueHandle) != 0)
    {
        return false;
    }

    for(uint8_t i=0; i<notifications[notification].size; i++)
    {
        if(pdTRUE != xQueueSend(soundQueueHandle,&(notifications[notification].samples[i]),10))
        {
            return false;
        }
    }

    return true;
}

void SoundNotificationsPlayInBlockingMode(SoundNotifications_t notification)
{
    for(uint8_t i=0; i<notifications[notification].size; i++)
    {
        BuzzerPlay(notifications[notification].samples[i].frequency, notifications[notification].samples[i].duration);

        while(BuzzerActive()){}

    }
}

void SoundNotificationsPlayAudio(SoundNotificationsAudioFiles_t audioFile)
{
    if(audioFile >= AD_AUDIO_FILES_COUNT)
    {
        return;
    }

    BuzzerPlayAudio(audioData[audioFile], AD_DATA_SIZE, AD_SAMPLE_RATE);
}



/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

