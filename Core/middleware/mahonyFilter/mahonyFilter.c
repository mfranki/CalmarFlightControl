/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/mahonyFilter/mahonyFilter.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/

#include "middleware/mahonyFilter/mahonyFilter.h"
#include "middleware/digitalFilter/digitalFilter.h"

#include "drivers/BMX055/BMX055.h"
#include "drivers/uart/uart.h"
#include "drivers/utils/utils.h"

#include "cmsis_os.h"
#include <main.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/
/*
#define MAG_GAIN (20.0f)
#define ACC_GAIN (0.6f)
*/
#define MAG_GAIN (5.0f)
#define ACC_GAIN (1.0f)

#define EARTH_GRAVITY_ACC (9.81f)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/
static  quaternion_t initialAccQuatVector = {.w = 0,
                                             .v = {0,0,-1}};

static quaternion_t initialMagQuatVector = {.w = 0,
                                            .v = {1,0,0}};

static quaternion_t orientation = {.w = 1,
                                .v = {0,0,0}};

static bool useMagnetometer = true;

static digitalFilterHandle_t filterHandleAx,filterHandleAy,filterHandleAz;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/


/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool MahonyFilterInit()
{
    /** 1Hz low pass filter **/
    float numerator[] = {0.000009825916820471736, 0.00001965183364094347, 0.000009825916820471736};
    float denominator[] = {1, -1.991114292201654, 0.991153595868936};

    if(!DigitalFilterCreateFilter(numerator, denominator, 2, &filterHandleAx)){return false;}
    if(!DigitalFilterCreateFilter(numerator, denominator, 2, &filterHandleAy)){return false;}
    if(!DigitalFilterCreateFilter(numerator, denominator, 2, &filterHandleAz)){return false;}
    return true;
}

void MahonyFilterTask()
{
    TickType_t lastTickTime = xTaskGetTickCount();

    uint32_t lastTimeCalled = 0;

    GetTimeElapsed(&lastTimeCalled, true);

    while(1)
    {
        bmx055Data_t imuData;
        Bmx055GetData(&imuData);
        float sampleTime = GetTimeElapsed(&lastTimeCalled, true);

        DigitalFilterProcess(filterHandleAx, imuData.ax, &(imuData.ax));
        DigitalFilterProcess(filterHandleAy, imuData.ay, &(imuData.ay));
        DigitalFilterProcess(filterHandleAz, imuData.az, &(imuData.az));

        /** calc estimated acc and mag vector positions based on last iteration **/
        quaternion_t accEstimate = QuatProd(QuatProd(QuatInv(orientation),initialAccQuatVector),orientation);
        quaternion_t magEstimate = QuatProd(QuatProd(QuatInv(orientation),initialMagQuatVector),orientation);

        /** calc mag vector part perpendicular to  acc **/
        vector_t mag = {imuData.mx,imuData.my,imuData.mz};
        mag = VectorNorm(mag);
        mag = VectorNorm(VectorDiff(mag,VectorMultiply(accEstimate.v,VectorDotProd(mag,accEstimate.v))));

        /** calculate position error between estimated and real**/
        quaternion_t magQuat = {.w = 0, .v = mag};
        quaternion_t accQuat = {.w = 0, .v = {imuData.ax,imuData.ay,imuData.az}};
        accQuat.v = VectorNorm(accQuat.v);

        quaternion_t accError = QuatMultiply(QuatProd(QuatInv(accEstimate),accQuat),ACC_GAIN);
        quaternion_t magError = QuatMultiply(QuatProd(QuatInv(magEstimate),magQuat),MAG_GAIN);

        quaternion_t gyroQuat = {.w = 0, .v = {imuData.gx,imuData.gy,imuData.gz}};

        if(!useMagnetometer)
        {
            magError.w = 0;
            magError.i = 0;
            magError.j = 0;
            magError.k = 0;
        }

        /** calculate current position based on estimation error and gyro step **/
        orientation = QuatSum(QuatMultiply(QuatProd(orientation,QuatSum(gyroQuat,QuatSum(accError,magError))),sampleTime/2),orientation);
        orientation = QuatNorm(orientation);

        vTaskDelayUntil(&lastTickTime,1);
    }
}


void MahonyFilterSetInitialMagVector(vector_t magVector)
{
    initialMagQuatVector.w = 0;
    initialMagQuatVector.v = VectorNorm(magVector);

    /** initial mag vector has to be perpendicular to acc vector **/
    initialMagQuatVector.v = VectorDiff(initialMagQuatVector.v ,VectorMultiply(initialAccQuatVector.v,VectorDotProd(magVector,initialAccQuatVector.v)));

    initialMagQuatVector.v = VectorNorm(initialMagQuatVector.v);
}

void MahonyFilterSetInitialAccVector(vector_t accVector)
{
    initialAccQuatVector.w = 0;
    initialAccQuatVector.v = accVector;
    initialAccQuatVector = QuatNorm(initialAccQuatVector);

    MahonyFilterSetInitialMagVector(initialMagQuatVector.v);
}

void MahonyFilterUseMagnetometer(bool useMag)
{
    useMagnetometer = useMag;
}

quaternion_t MahonyFilterGetOrientation()
{
    return orientation;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

