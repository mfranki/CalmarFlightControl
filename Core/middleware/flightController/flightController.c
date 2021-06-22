/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/flightController/flightController.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 18, 2021
 ****************************************************************************/
#include "drivers/utils/utils.h"
#include "drivers/uart/uart.h"
#include "drivers/eeprom/eeprom.h"
#include "drivers/motors/motors.h"

#include "middleware/flightController/flightController.h"
#include "middleware/quaternion/quaternion.h"
#include "middleware/mahonyFilter/mahonyFilter.h"
#include "middleware/radioStatus/radioStatus.h"
#include "middleware/vector/vector.h"
#include "middleware/quaternion/quaternion.h"
#include "middleware/pid/pid.h"
#include "middleware/remoteSettings/remoteSettings.h"
#include "middleware/memory/memory.h"
#include "middleware/radioStatus/radioStatus.h"
#include "middleware/digitalFilter/digitalFilter.h"

#include "app/deviceManager/deviceManager.h"

#include "math.h"
#include "cmsis_os.h"
/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/


#define ROLL_MAX_ANGLE_D  (20.0f)   ///< ROLL angle range +-10 deg
#define PITCH_MAX_ANGLE_D (20.0f)   ///< PITCH angle range +-10 deg
#define YAW_MAX_ANGLE_DPS (40.0f)   ///< YAW angle velocity range +-40 deg/s

#define YAW_MIN_INCREMENT_D (2.0f)  ///< deg/s minimal yaw stick value above which yaw is affected
#define ROLL_PITCH_MIN_VALUE_D (0.2f)   ///< deg bellow this value roll/pitch will be 0

#define MIN_THROTTLE    (0.3f)
#define MAX_THROTTLE    (0.7f)
#define MAX_BALANCE_XY  (0.1f)
#define MAX_BALANCE_Z   (0.1f)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static volatile float yaw = 0;

static pidHandle_t pidHandleX = 0;
static pidHandle_t pidHandleY = 0;
static pidHandle_t pidHandleZ = 0;

static digitalFilterHandle_t filterHandleAz;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief calculates target orientation based on data from radio
 *        target orientation is calculated as :
 *        rotation around Z axis -> simultaneous rotation around X and Y axes
 * @param [in] sampleTime
 * @return target rotation as quaternion
 */
static quaternion_t CalcTargetOrientation(float sampleTime);

/**@brief sets PID x,y,z parameters to values stored in remote settings
 *
 * @return true if successful
 */
static bool UpdatePidParams();

/**@brief when any setting in remoteSettings is changed this callback is called to update it locally
 */
static void SettingsUpdateCallback();

/**@brief mixes output data fromPId regulators and throttle to get values that are send to the motors
 *
 * @param [in] throttle
 * @param [in] x - output value from X axis PID regulator
 * @param [in] y - output value from Y axis PID regulator
 * @param [in] z - output value from Z axis PID regulator
 */
static void MixSignals(float throttle, float x, float y, float z);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool FlightControllerInit()
{
    if(!PidInit(&pidHandleX,0,0,0,0)) { return false; }
    if(!PidInit(&pidHandleY,0,0,0,0)) { return false; }
    if(!PidInit(&pidHandleZ,0,0,0,0)) { return false; }

    if(!UpdatePidParams()){return false;}

    if(!RemoteSettingsAddUpdateCallback(&SettingsUpdateCallback)){return false;}

    /** 1Hz low pass filter for z axis**/
    float numerator[] = {0.003621681514929, 0.007243363029857, 0.003621681514929};
    float denominator[] = {1, -1.822694925196308, 0.837181651256023};

    if(!DigitalFilterCreateFilter(numerator, denominator, 2, &filterHandleAz)){return false;}

    return true;
}

void FlightControllerTask()
{
    uint32_t previousWakeTime = osKernelSysTick();
    uint32_t lastTimeCalled = 0;
    while(1)
    {
        if(DeviceManagerGetOperatingMode() != DEVICE_FLIGHT &&
           DeviceManagerGetOperatingMode() != DEVICE_HOMING)
        {
            vTaskSuspend(NULL);
            GetTimeElapsed(&lastTimeCalled, true);
            previousWakeTime = osKernelSysTick();

            vector_t startingOrientation = QuatTranslateToRotationVector(MahonyFilterGetOrientation());
            yaw = startingOrientation.z;
        }

        float sampleTime = GetTimeElapsed(&lastTimeCalled, true);

        quaternion_t targetOrientation = CalcTargetOrientation(sampleTime);

        quaternion_t currentOrientation = MahonyFilterGetOrientation();

        vector_t orientationError = QuatTranslateToRotationVector(QuatProd(QuatInv(currentOrientation),targetOrientation));

        DigitalFilterProcess(filterHandleAz, orientationError.z, &(orientationError.z));

        float throttle = 0;

        if(DeviceManagerGetOperatingMode() == DEVICE_FLIGHT)
        {
            throttle = RadioStatusGetChannelData(RADIO_THROTTLE_CHANNEL);
        } else if(DeviceManagerGetOperatingMode() == DEVICE_HOMING)
        {
            throttle = 0;
        }

        MixSignals(throttle,
            PidCalc(pidHandleX, orientationError.x),
            PidCalc(pidHandleY, orientationError.y),
            PidCalc(pidHandleZ, orientationError.z));

        osDelayUntil(&previousWakeTime,20);
    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static quaternion_t CalcTargetOrientation(float sampleTime)
{
    static bool yawIncremented = false;

    float yawIncrement = -RadioStatusGetChannelData(RADIO_YAW_CHANNEL)*YAW_MAX_ANGLE_DPS*(M_PI)/(180.0f)*sampleTime;

    if(fabs(yawIncrement) > YAW_MIN_INCREMENT_D*M_PI/180*sampleTime)
    {
        yaw += yawIncrement;
        yawIncremented = true;
    } else if(yawIncremented){
        vector_t currentOrientation = QuatTranslateToRotationVector(MahonyFilterGetOrientation());
        yaw = currentOrientation.z;
        yawIncremented = false;
    }

    if(yaw > M_PI)
    {
        yaw = yaw - 2*M_PI;
    } else if(yaw < -M_PI)
    {
        yaw = yaw + 2*M_PI;
    }

    vector_t yawRotation = {0, 0, yaw};
    vector_t rpRotation = {RadioStatusGetChannelData(RADIO_ROLL_CHANNEL)*ROLL_MAX_ANGLE_D*M_PI/180,
                           RadioStatusGetChannelData(RADIO_PITCH_CHANNEL)*PITCH_MAX_ANGLE_D*M_PI/180,
                           0};

    if(fabs(rpRotation.x) < ROLL_PITCH_MIN_VALUE_D*M_PI/180)
    {
        rpRotation.x = 0;
    }

    if(fabs(rpRotation.y) < ROLL_PITCH_MIN_VALUE_D*M_PI/180)
    {
        rpRotation.y = 0;
    }

    return QuatProd(QuatTranslateVectorToQuaternion(yawRotation),QuatTranslateVectorToQuaternion(rpRotation));
}

static bool UpdatePidParams()
{
    float pxy, ixy, dxy, pz, iz, dz, n;

    if(!RemoteSettingsGetVariable(RS_PID_XY_P, &pxy)){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_XY_I, &ixy)){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_XY_D, &dxy)){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_Z_P , &pz )){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_Z_I , &iz )){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_Z_D , &dz )){return false;}
    if(!RemoteSettingsGetVariable(RS_PID_N   , &n  )){return false;}

    if(!PidSetParam(pidHandleX, PID_P, pxy)){return false;}
    if(!PidSetParam(pidHandleX, PID_I, ixy)){return false;}
    if(!PidSetParam(pidHandleX, PID_D, dxy)){return false;}
    if(!PidSetParam(pidHandleX, PID_N, n  )){return false;}

    if(!PidSetParam(pidHandleY, PID_P, pxy)){return false;}
    if(!PidSetParam(pidHandleY, PID_I, ixy)){return false;}
    if(!PidSetParam(pidHandleY, PID_D, dxy)){return false;}
    if(!PidSetParam(pidHandleY, PID_N, n  )){return false;}

    if(!PidSetParam(pidHandleZ, PID_P, pz )){return false;}
    if(!PidSetParam(pidHandleZ, PID_I, iz )){return false;}
    if(!PidSetParam(pidHandleZ, PID_D, dz )){return false;}
    if(!PidSetParam(pidHandleZ, PID_N, n  )){return false;}

    return true;
}

static void SettingsUpdateCallback()
{
    UpdatePidParams();
}

static void MixSignals(float throttle, float x, float y, float z)
{
    if(throttle>1)
    {
        throttle = 1;
    }

    throttle *= MAX_THROTTLE;

    float balanceCut = throttle<MIN_THROTTLE ? throttle/MIN_THROTTLE : 1;

    if(fabs(x) > MAX_BALANCE_XY*balanceCut)
    {
        x = ((float)((x>0)*2-1))*MAX_BALANCE_XY*balanceCut;
    }

    if(fabs(y) > MAX_BALANCE_XY*balanceCut)
    {
        y = ((float)((y>0)*2-1))*MAX_BALANCE_XY*balanceCut;
    }

    if(fabs(z) > MAX_BALANCE_Z*balanceCut)
    {
        z = ((float)((z>0)*2-1))*MAX_BALANCE_Z*balanceCut;
    }

    MotorsSet(MOTORS_FRONT_RIGHT,  throttle-x-y+z);
    MotorsSet(MOTORS_FRONT_LEFT ,  throttle+x-y-z);
    MotorsSet(MOTORS_BACK_LEFT  ,  throttle+x+y+z);
    MotorsSet(MOTORS_BACK_RIGHT ,  throttle-x+y-z);
}
