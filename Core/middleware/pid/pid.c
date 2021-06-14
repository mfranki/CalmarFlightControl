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

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef struct{
    float p;
    float i;
    float d;
    float filterCoefficient;

    float integral;
}pidData_t;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

pidHandle_t PidInit(float p, float i, float d, float filterCoefficient)
{

}

float PidCalc(pidHandle_t pidHandle, float input)
{

}

float PidCalcExternVel(pidHandle_t pidHandle, float input, float inputVel)
{

}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

