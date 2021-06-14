/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/remoteSettings/remoteSettings.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 6, 2021
 ****************************************************************************/

#include "middleware/remoteSettings/remoteSettings.h"

#include "app/deviceManager/deviceManager.h"

#include "middleware/soundNotifications/soundNotifications.h"
#include "middleware/radioStatus/radioStatus.h"

#include <stdlib.h>
#include <cmsis_os.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define SWITCH_TRH_0_1 (0.25f)  ///< bellow this threshold switch is set to 0, above is 1 and 2
#define SWITCH_TRH_1_2 (0.75f)  ///< above this threshold switch is set to 2, bellow is 0 and 1

#define DIAL_ITEM_VALUE_START_MIN (0.45f)
#define DIAL_ITEM_VALUE_START_MAX (0.55f)

#define ITEM_SELECTION_COUNTER_MAX (10u)
#define VALUE_READOUT_COUNTER_MAX (9u)
#define VALUE_CHANGE_CONTINUE_COUNTER_MAX (8u)

#define VALUE_CHANGE_SCALE (20.0f)

#define DIGITS_IN_INT32 (10u)       ///< decimial digits in int32_t

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef enum{
    SWITCH_OFF,
    SWITCH_MENU_ITEM,
    SWITCH_MENU_ITEM_VALUE
}switchState_t;

/**@brief all settings variables
 */
static float variables[RS_VARIABLES_COUNT];

static void (**updateCallbacks)() = NULL;
static uint32_t updateCallbacksCount = 0;


/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief checks switch state against thresholds and it value
 *
 * @return switch state 0,1,2
 */
static switchState_t GetSwitchState();

/**@brief checks dial value against thresholds and calculates menu
 *        item fitting detected range
 *
 *
 * @param [in] dialValue
 * @return menu item number, is valid only when switch state == SWITCH_MENU_ITEM
 */
static settingsVariable_t GetSelectedMenuItem(float dialValue);

/**@brief checks if dial is for ITEM_SELECTION_COUNTER_MAX samples on one value
 *        then assumes item to be selected
 *
 * @param [in] switchState
 * @param [in] dialValue
 * @param [out] currentSelectedVariable - changed when item is selected
 * @return true if new item was selected
 */
static bool DetectItemSelection(switchState_t switchState, float dialValue, settingsVariable_t *currentSelectedVariable);

/**@brief check if switch moved from SWITCH_MENU_ITEM_VALUE to SWITCH_MENU_ITEM_VALUE
 *        and stayed there for VALUE_READOUT_COUNTER_MAX cycles
 *
 * @param [in] switchState
 * @param [in] dialValue
 * @return true if value readout detected
 */
static bool DetectValueReadout(switchState_t switchState, float dialValue);

/**@brief checks if dial is on 50% when changing variable value so that there is always space for moving dial
 *
 * @param [in] switchState
 * @param [in] dialValue
 * @return true if dial is in range DIAL_ITEM_VALUE_START_MIN::DIAL_ITEM_VALUE_START_MAX
 */
static bool DetectValueChangeStartingPoint(switchState_t switchState, float dialValue);

/**@brief checks when switch was moved to SWITCH_OFF
 *
 * @param [in] switchState
 * @return true if was moved to SWITCH_OFF
 */
static bool DetectValueUpdate(switchState_t switchState);

/**@brief Reads out float as series of numbers nad dot
 *
 * @param [in]f
 */
static void PlayFloat(float f);

/**@brief reads out UINT as series of numbers
 *
 * @param [in]integer
 * @param [in]minDigits
 */
static void PlayUint(uint32_t integer, uint8_t minDigits);

/**@brief plays number as sum of fives and ones
 *        long beep = five
 *        short beep = 1
 * @param [in] number
 */
static void PlayNumber(settingsVariable_t number);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void RemoteSettingsTask()
{
    settingsVariable_t currentSelectedVariable = 0;

    float valueChangeStartingPoint = 0.0f;
    float valueChangeDialReference = 0.5f;
    bool valueChangeStartingPointReady = false;

    while(1)
    {
        switchState_t switchState = GetSwitchState();
        float dialValue = RadioStatusGetChannelData(RADIO_DIAL_CHANNEL);

        if(DetectItemSelection(switchState,dialValue,&currentSelectedVariable))
        {
            PlayNumber(currentSelectedVariable+1);
        }

        if(DetectValueChangeStartingPoint(switchState,dialValue) && !valueChangeStartingPointReady)
        {
            SoundNotificationsPlay(SN_SETTINGS_MENU_ITEM_1);
            osDelay(500);

            SoundNotificationsPlay(SN_SETTINGS_MENU_ITEM_1);
            osDelay(200);
            SoundNotificationsPlay(SN_SETTINGS_MENU_ITEM_1);
            valueChangeDialReference = RadioStatusGetChannelData(RADIO_DIAL_CHANNEL);
            valueChangeStartingPoint = variables[currentSelectedVariable];
            valueChangeStartingPointReady = true;
        }

        if(switchState == SWITCH_MENU_ITEM_VALUE && valueChangeStartingPointReady)
        {
            variables[currentSelectedVariable] = valueChangeStartingPoint-VALUE_CHANGE_SCALE*(dialValue-valueChangeDialReference);
        } else {
            valueChangeStartingPointReady = false;
        }

        if(DetectValueReadout(switchState, dialValue))
        {
            PlayFloat(variables[currentSelectedVariable]);
            osDelay(2000);
        }

        if(DetectValueUpdate(switchState))
        {
            for(uint8_t i=0; i<updateCallbacksCount; i++)
            {
                updateCallbacks[i]();
            }
        }

        do{
            osDelay(100);
        }while(DeviceManagerGetOperatingMode() != DEVICE_SETTINGS);
    }
}

bool RemoteSettingsAddUpdateCallback(void (*updateCallback)(void))
{
    if(updateCallback == NULL)
    {
        return false;
    }

    void* ptr = NULL;

    if(updateCallbacks == NULL)
    {
        ptr = malloc(sizeof(*updateCallbacks));
    } else {
        ptr = realloc(updateCallbacks,sizeof(*updateCallbacks));
    }

    updateCallbacks = ptr!=NULL ? ptr : updateCallbacks;
    updateCallbacks[updateCallbacksCount] = updateCallback;
    updateCallbacksCount++;

    return true;
}

bool RemoteSettingsSetVariable(settingsVariable_t variable, float value)
{
    if(variable >= RS_VARIABLES_COUNT)
    {
        return false;
    }
    variables[variable] = value;

    return true;
}

bool RemoteSettingsGetVariable(settingsVariable_t variable, float *value)
{
    if(variable >= RS_VARIABLES_COUNT)
    {
        return false;
    }
    *value = variables[variable];

    return true;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static switchState_t GetSwitchState()
{
    float switchChannelData = RadioStatusGetChannelData(RADIO_SWITCH_CHANNEL);

    if(SWITCH_TRH_0_1 >= switchChannelData)
    {
        return SWITCH_OFF;
    }

    if(SWITCH_TRH_1_2 < switchChannelData)
    {
        return SWITCH_MENU_ITEM_VALUE;
    }

    return SWITCH_MENU_ITEM;
}

static settingsVariable_t GetSelectedMenuItem(float dialValue)
{
    float step = 1/(float)RS_VARIABLES_COUNT;
    settingsVariable_t selectedVariable = (settingsVariable_t)(dialValue/step);

    return selectedVariable >= RS_VARIABLES_COUNT ? RS_VARIABLES_COUNT-1 : selectedVariable;
}


static bool DetectItemSelection(switchState_t switchState, float dialValue, settingsVariable_t *currentSelectedVariable)
{
    static settingsVariable_t lastVariable = 0;
    static uint8_t itemSelectionCounter = 0;

    if(switchState != SWITCH_MENU_ITEM)
    {
        lastVariable = 0;
        itemSelectionCounter = 0;
        return false;
    }

    settingsVariable_t currentVariable = GetSelectedMenuItem(dialValue);

    if(itemSelectionCounter == ITEM_SELECTION_COUNTER_MAX)
    {
        *currentSelectedVariable = currentVariable;
        itemSelectionCounter++;
        return true;
    } else if(lastVariable==currentVariable)
    {
        itemSelectionCounter += itemSelectionCounter>ITEM_SELECTION_COUNTER_MAX ? 0 : 1;
    } else {
        itemSelectionCounter = 0;
    }

    lastVariable = currentVariable;

    return false;
}

static bool DetectValueReadout(switchState_t switchState, float dialValue)
{
    static switchState_t lastSwitchState = SWITCH_OFF;
    static uint8_t valueReadoutCounter = 0;

    if(switchState != SWITCH_MENU_ITEM)
    {
        valueReadoutCounter = 0;
    } else if(lastSwitchState == SWITCH_MENU_ITEM_VALUE || valueReadoutCounter > 0)
    {
        valueReadoutCounter += valueReadoutCounter>VALUE_READOUT_COUNTER_MAX ? 0 : 1;
    }

    lastSwitchState = switchState;

    if(valueReadoutCounter == VALUE_READOUT_COUNTER_MAX)
    {
        return true;
    }

    return false;
}

static bool DetectValueChangeStartingPoint(switchState_t switchState, float dialValue)
{
    if(DIAL_ITEM_VALUE_START_MIN < dialValue &&
       DIAL_ITEM_VALUE_START_MAX > dialValue &&
       switchState == SWITCH_MENU_ITEM_VALUE)
    {
        return true;
    }
    return false;
}

static bool DetectValueUpdate(switchState_t switchState)
{
    static switchState_t lastSwitchState = SWITCH_OFF;
    if(switchState == SWITCH_OFF && lastSwitchState != SWITCH_OFF)
    {
        lastSwitchState = switchState;
        return true;
    }

    lastSwitchState = switchState;
    return false;
}

static void PlayFloat(float f)
{
    if(f < 0)
    {
        SoundNotificationsPlayAudio(SN_MINUS);
        osDelay(500);
        f = -f;
    }

    uint32_t wholes = (uint32_t)f;
    PlayUint(wholes,0);
    SoundNotificationsPlayAudio(SN_DOT);
    osDelay(500);
    uint32_t parts = (uint32_t)((f-(float)wholes)*100);

    PlayUint(parts,2);
}

static void PlayUint(uint32_t integer, uint8_t minDigits)
{
    uint8_t digits[DIGITS_IN_INT32] = {0,0,0,0,0,0,0,0,0,0};
    uint8_t firstDigit = DIGITS_IN_INT32-1;

    for(uint8_t i=DIGITS_IN_INT32-1; i>0; i--)
    {
        digits[i] = integer-(integer/10)*10;
        integer /= 10;

        if(digits[i] != 0)
        {
            firstDigit = i;
        }

    }

    uint8_t digit = DIGITS_IN_INT32-minDigits<firstDigit ? DIGITS_IN_INT32-minDigits : firstDigit;

    for(uint8_t i=digit; i<DIGITS_IN_INT32;i++)
    {
        SoundNotificationsPlayAudio(digits[i]);
        osDelay(500);
    }
}

static void PlayNumber(uint8_t number)
{
    for(uint8_t i=0; i<number/5; i++)  ///< play long impulses first
    {
        SoundNotificationsPlay(SN_SETTINGS_MENU_ITEM_5);
        osDelay(600);
    }

    for(uint8_t i=0; i<number%5; i++)  ///< play remaining short impulses
    {
        SoundNotificationsPlay(SN_SETTINGS_MENU_ITEM_1);
        osDelay(300);
    }
}
