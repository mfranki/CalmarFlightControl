/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/mahonyFilter/mahonyFilter.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/

#include "middleware/mahonyFilter/mahonyFilter.h"
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

#define MAG_GAIN (20.0f)
#define ACC_GAIN (0.6f)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/
static  quaternion_t initialAccQuatVector = {.w = 0,
                                             .v = {0,0,-1}};

static quaternion_t initialMagQuatVector = {.w = 0,
                                            .v = {1,0,0}};

static quaternion_t position = {.w = 1,
                                .v = {0,0,0}};

static bool useMagnetometer = true;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void MahonyFilterTask()
{
    bmx055Data_t imuData;

    TickType_t lastTickTime = xTaskGetTickCount();

    uint32_t lastTimeCalled = 0;

    GetTimeElapsed(&lastTimeCalled, true);

    while(1)
    {
        Bmx055GetData(&imuData);
        float sampleTime = GetTimeElapsed(&lastTimeCalled, true);

        /** calc estimated acc and mag vector positions based on last iteration **/
        quaternion_t accEstimate = QuatProd(QuatProd(QuatInv(position),initialAccQuatVector),position);
        quaternion_t magEstimate = QuatProd(QuatProd(QuatInv(position),initialMagQuatVector),position);

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
        gyroQuat.v = VectorMultiply(gyroQuat.v,M_PI/180);

        if(!useMagnetometer)
        {
            magError.w = 0;
            magError.i = 0;
            magError.j = 0;
            magError.k = 0;
        }

        position = QuatSum(QuatMultiply(QuatProd(position,QuatSum(gyroQuat,QuatSum(accError,magError))),sampleTime/2),position);
        position = QuatNorm(position);

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

quaternion_t MahonyFilterGetPosition()
{
    return position;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

