/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/vector/vector.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 1, 2021
 ****************************************************************************/

#include "middleware/vector/vector.h"

#include <math.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/



/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

vector_t VectorSum(vector_t v1, vector_t v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vector_t VectorDiff(vector_t v1, vector_t v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

vector_t VectorNegative(vector_t v)
{
    v.x = -v.x;
    v.y = -v.y;
    v.z = -v.z;
    return v;
}

vector_t VectorMultiply(vector_t v, float a)
{
    v.x *= a;
    v.y *= a;
    v.z *= a;
    return v;
}

float VectorDotProd(vector_t v1, vector_t v2)
{
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

vector_t VectorCrossProd(vector_t v1, vector_t v2)
{
    vector_t v;

    v.x = v1.y*v2.z-v1.z*v2.y;
    v.y = v1.z*v2.x-v1.x*v2.z;
    v.z = v1.x*v2.y-v1.y*v2.x;

    return v;
}

float VectorLength(vector_t v)
{
    return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

vector_t VectorNorm(vector_t v)
{
    float length = VectorLength(v);

    if(length == 0)
    {
        return v;
    }

    return VectorMultiply(v,1/length);
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

