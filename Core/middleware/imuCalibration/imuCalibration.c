/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/imuCalibration/imuCalibration.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date 1 cze 2021
 ****************************************************************************/

#include "app/deviceManager/devicemanager.h"

#include "middleware/soundNotifications/soundNotifications.h"
#include "middleware/radioStatus/radioStatus.h"
#include "middleware/remoteSettings/remoteSettings.h"
#include "middleware/vector/vector.h"
#include "middleware/mahonyFilter/mahonyFilter.h"

#include "drivers/uart/uart.h"
#include "drivers/BMX055/BMX055.h"
#include "drivers/utils/utils.h"

#include <cmsis_os.h>
#include <math.h>
#include <main.h>
#include <middleware/imuCalibration/imuCalibration.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define ACC_GYRO_CALIBRATION_SAMPLES (1000u)    ///< amount of samples taken for acc gyro calibration
#define ACC_Z_TARGET_VALUE (-9.81f)              ///< earth gravity acceleration

#define AXIS_ANGLE_TOLERANCE (15*M_PI/180) ///< +-15 deg
#define AXIS_DOWN_TOLREANCE (1-cos(AXIS_ANGLE_TOLERANCE))   ///< tolerance of gravity vector for deciding which axis faces down

#define SAMPLES_PER_ROTATION (36U)  ///< amount of samples gathered per half axis when calibrating mag

/// TODO: this macro needs to be replaced by more sophisticated algorithm that decides which data is faulty
#define MAG_VALIDITY_TRH (70.0f)    ///< data above this trh is assumed to be faulty

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef enum{
    PLUS_X, ///< PLUS_X
    MINUS_X,///< MINUS_X
    PLUS_Y, ///< PLUS_Y
    MINUS_Y,///< MINUS_Y
    PLUS_Z, ///< PLUS_Z
    MINUS_Z,///< MINUS_Z
    NONE,   ///< NONE
}axesDirections_t;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief checks which half axis is pointing down according to AXIS_DOWN_TOLREANCE
 *
 * @return half axis pointing down
 */
static axesDirections_t GetAxisPointingDown();

/**@brief integrates gyro position,
 *        !! because of broken gyroscope
 *           for z axis uses magnetometer data so magOffset needs to be
 *           calculated at least partially to provide somewhat accurate readings on z axis
 *
 *       !! data from this function is accurate only when pointing one of the axes down, and reseting before
 *
 * @param [in] reset - resets calculated position to {0,0,0}
 * @param [in] magOffset - offset for mag data, does not affect BMX055.h library
 * @return calculated orientation
 */
static vector_t IntegrateGyro(bool reset, vector_t magOffset);

/**@brief performs calibration sequence for magnetometer
 *
 * @return true if sequence was finished uninterrupted
 */
static bool CalibrateMagnetometer();

/**@brief calculates magOffset based on uncalibrated mag data and amount of axes done
 *
 * @param [in] sampledMagData
 * @param [in] axesDone
 * @return magnetometer offsets
 */
static vector_t CalcMagOffset(vector_t *sampledMagData, axesDirections_t axesDone);

/**@brief performs modulo function same as in matlab
 *
 * @param [in] x
 * @param [in] y
 * @return mod(x,y)
 */
static float MatlabMod(float x, float y);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void ImuCalibrationTask()
{
    while(1)
    {
        vTaskSuspend(NULL);

        SoundNotificationsPlay(SN_CALIBRATION_START);
        osDelay(500);

        /** calibrate acc ang gyro **/
        Bmx055SetAccOffsets(0,0,0);
        Bmx055SetGyroOffsets(0,0,0);

        BMX055CalibrateAccGyro();

        if(DeviceManagerGetOperatingMode() != DEVICE_CALIBRATION)
        {
            continue;
        }

        vector_t accSum = {0,0,0};
        vector_t gyroSum = {0,0,0};

        for(uint32_t i=0; i<ACC_GYRO_CALIBRATION_SAMPLES; i++)
        {
            bmx055Data_t imuData;
            Bmx055GetData(&imuData);
            accSum.x += imuData.ax;
            accSum.y += imuData.ay;
            accSum.z += imuData.az;
            gyroSum.x += imuData.gx;
            gyroSum.y += imuData.gy;
            gyroSum.z += imuData.gz;
            osDelay(1);
        }

        accSum = VectorMultiply(accSum,1/((float)ACC_GYRO_CALIBRATION_SAMPLES));
        gyroSum = VectorMultiply(gyroSum,1/((float)ACC_GYRO_CALIBRATION_SAMPLES));

        Bmx055SetAccOffsets(accSum.x,accSum.y,accSum.z-ACC_Z_TARGET_VALUE);
        Bmx055SetGyroOffsets(gyroSum.x,gyroSum.y,gyroSum.z);

        float calibration = 0;
        RemoteSettingsGetVariable(RS_CALIBRATION, &calibration);
        if(calibration < 0)
        {
            while(true != SoundNotificationsPlay(SN_CALIBRATION_FINISHED)){}
            continue;
        }

        if(DeviceManagerGetOperatingMode() != DEVICE_CALIBRATION)
        {
            continue;
        }

        /** calibrate magnetometer **/

        Bmx055SetMagOffsets(0,0,0);
        if(!CalibrateMagnetometer())
        {
            continue;
        }

        while(true != SoundNotificationsPlay(SN_CALIBRATION_FINISHED)){}


    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static axesDirections_t GetAxisPointingDown()
{
    bmx055Data_t data;
    if(!Bmx055GetData(&data))
    {
        return NONE;
    }

    vector_t acc = {data.ax,data.ay,data.az};
    acc = VectorNorm(acc);

    for(uint8_t axis=0; axis<3; axis++)
    {
        for(int8_t dir=1; dir>-1; dir--)
        {
            if((((float*)&acc)[axis] > ((float)(dir*2-1)-AXIS_DOWN_TOLREANCE)) &&
               (((float*)&acc)[axis] < ((float)(dir*2-1)+AXIS_DOWN_TOLREANCE)))
            {
                return 2*axis+dir;
            }
        }
    }

    return NONE;
}

static vector_t IntegrateGyro(bool reset, vector_t magOffset)
{
    static vector_t pos = {0,0,0};
    static uint32_t lastTimeCalled = 0;
    static float magAngle = 0;

    float timeElapsed = GetTimeElapsed(&lastTimeCalled, true);

    bmx055Data_t data;
    if(!Bmx055GetData(&data))
    {
        return pos;
    }

    if(reset)
    {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        magAngle = atan2(data.my,data.mx);

        timeElapsed = GetTimeElapsed(&lastTimeCalled, true);
        return pos;
    }

    vector_t gyro = {data.gx,data.gy,0/*data.gz*/};     ///< broken gyro z

    pos = VectorSum(pos,VectorMultiply(gyro,timeElapsed));


    /** this code is there only because of broken gyroscope **/
    data.mx -= magOffset.x;
    data.my -= magOffset.y;
    data.mz -= magOffset.z;

    if(pos.z > 7*M_PI/4)
    {
        pos.z = atan2(-data.mx*sin(magAngle)+data.my*cos(magAngle),data.mx*cos(magAngle)+data.my*sin(magAngle))+2*M_PI;
    } else if(pos.z < -7*M_PI/4)
    {
        pos.z = atan2(-data.mx*sin(magAngle)+data.my*cos(magAngle),data.mx*cos(magAngle)+data.my*sin(magAngle))-2*M_PI;
    } else if(pos.z > 3*M_PI/4)
    {
        pos.z = MatlabMod(atan2(-data.mx*sin(magAngle)+data.my*cos(magAngle),data.mx*cos(magAngle)+data.my*sin(magAngle)),2*M_PI);
    } else if(pos.z < -3*M_PI/4)
    {
        pos.z = MatlabMod(atan2(-data.mx*sin(magAngle)+data.my*cos(magAngle),data.mx*cos(magAngle)+data.my*sin(magAngle)),2*M_PI)-2*M_PI;
    } else {
        pos.z = atan2(-data.mx*sin(magAngle)+data.my*cos(magAngle),data.mx*cos(magAngle)+data.my*sin(magAngle));
    }

    return pos;
}

static bool CalibrateMagnetometer()
{
    uint8_t sampleCounter = 0;
    axesDirections_t currentAxis = PLUS_X;

    vector_t samples[6*SAMPLES_PER_ROTATION];

    bool newAxisFound = false;

    vector_t magTempOffset = {0,0,0};

    IntegrateGyro(true,magTempOffset);

    while(currentAxis != NONE)
    {
        if(DeviceManagerGetOperatingMode() != DEVICE_CALIBRATION)
        {
            return false;
        }

        osDelay(10);

        vector_t pos = IntegrateGyro(!newAxisFound,magTempOffset);

        if(currentAxis != GetAxisPointingDown())
        {
            SoundNotificationsPlay(SN_MAGNETOMETER_CALIBRATION_INVALID_AXIS);
            continue;
        } else {
            newAxisFound = true;
        }

        float axisData = 0;

        if(currentAxis == PLUS_X ||
           currentAxis == MINUS_X)
        {
            axisData = pos.x;
        } else if(currentAxis == PLUS_Y ||
                  currentAxis == MINUS_Y)
        {
            axisData = pos.y;
        } else if(currentAxis == PLUS_Z ||
                  currentAxis == MINUS_Z)
        {
            axisData = pos.z;
        }

        if(fabs(axisData) > 2*M_PI/(float)SAMPLES_PER_ROTATION*(float)sampleCounter)
        {
            bmx055Data_t data;
            if(!Bmx055GetData(&data))
            {
                continue;
            }

            if(fabs(data.mx) > MAG_VALIDITY_TRH ||
               fabs(data.my) > MAG_VALIDITY_TRH ||
               fabs(data.mz) > MAG_VALIDITY_TRH)
            {
                continue;
            }

            samples[currentAxis*SAMPLES_PER_ROTATION+sampleCounter].x = data.mx;
            samples[currentAxis*SAMPLES_PER_ROTATION+sampleCounter].y = data.my;
            samples[currentAxis*SAMPLES_PER_ROTATION+sampleCounter].z = data.mz;

            sampleCounter++;
        }


        if(sampleCounter >= SAMPLES_PER_ROTATION)
        {
            while(true != SoundNotificationsPlay(SN_MAGNETOMETER_CALIBRATION_AXIS_DONE)){}


            /** this if is there only because of broken gyroscope **/
            if(currentAxis == MINUS_Y)
            {
                magTempOffset = CalcMagOffset(samples, PLUS_Z);
            }

            currentAxis++;
            sampleCounter = 0;
            newAxisFound = false;
            osDelay(1000);
        }
    }

    magTempOffset = CalcMagOffset(samples, NONE);

    Bmx055SetMagOffsets(magTempOffset.x,magTempOffset.y,magTempOffset.z);
    return true;
}

static vector_t CalcMagOffset(vector_t *sampledMagData, axesDirections_t axesDone)
{
    vector_t sum = {0,0,0};
    for(uint32_t i=0; i<SAMPLES_PER_ROTATION*axesDone; i++)
    {

        sum = VectorSum(sum,sampledMagData[i]);
    }
    return VectorMultiply(sum,1/(float)(SAMPLES_PER_ROTATION*axesDone));
}

static float MatlabMod(float x, float y)
{
    float f = fmod(x,y);
    return f + y*(f<0);
}
