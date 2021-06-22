/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/pid/pid.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 2, 2021
 ****************************************************************************/

#include "middleware/pid/pid.h"

#include "drivers/utils/utils.h"

#include <stdlib.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define YVk_1 (((pidData_t*)pidHandle)->prevVelOut)
#define YIk_1 (((pidData_t*)pidHandle)->prevIntegralOut)
#define Uk_1  (((pidData_t*)pidHandle)->prevIn)
#define N     (((pidData_t*)pidHandle)->filterCoefficient)
#define P     (((pidData_t*)pidHandle)->p)
#define I     (((pidData_t*)pidHandle)->i)
#define D     (((pidData_t*)pidHandle)->d)


/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef struct{
    float p;
    float i;
    float d;
    float filterCoefficient;

    float prevIntegralOut;
    float prevVelOut;
    float prevIn;

    uint32_t lastTimeCalled;
}pidData_t;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool PidInit(pidHandle_t *pidHandle, float p, float i, float d, float filterCoefficient)
{
    *pidHandle = (uint32_t)malloc(sizeof(pidData_t));

    if(*pidHandle == 0)
    {
        return false;
    }

    ((pidData_t*)*pidHandle)->p = p;
    ((pidData_t*)*pidHandle)->i = i;
    ((pidData_t*)*pidHandle)->d = d;
    ((pidData_t*)*pidHandle)->filterCoefficient = filterCoefficient;
    ((pidData_t*)*pidHandle)->prevIntegralOut = 0;
    ((pidData_t*)*pidHandle)->prevIn = 0;
    ((pidData_t*)*pidHandle)->prevVelOut = 0;
    ((pidData_t*)*pidHandle)->lastTimeCalled = 0;

    GetTimeElapsed(&(((pidData_t*)*pidHandle)->lastTimeCalled), true);

    return true;
}

float PidCalc(pidHandle_t pidHandle, float input)
{
    float ts = GetTimeElapsed(&(((pidData_t*)pidHandle)->lastTimeCalled), true);

    /** CALCULATE VELOCITY **/
    YVk_1 = -YVk_1*(N*ts-1) - Uk_1*N + input*N;

    /** INTEGRATE INPUT **/
    YIk_1 = ts*Uk_1 + YIk_1;

    Uk_1 = input;

    return P*input + I*YIk_1 + D*YVk_1;
}

bool PidSetParam(pidHandle_t pidHandle, pidParameters_t param, float value)
{
    if(pidHandle == 0 || param > PID_N)
    {
        return false;
    }
    *(float*)(pidHandle+4*param) = value;

    return true;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

