/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/quaternion/quaternion.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 1, 2021
 ****************************************************************************/

#include "middleware/quaternion/quaternion.h"

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

quaternion_t QuatSum(quaternion_t q1, quaternion_t q2)
{
    q1.w += q2.w;
    q1.i += q2.i;
    q1.j += q2.j;
    q1.k += q2.k;

    return q1;
}

quaternion_t QuatDiff(quaternion_t q1, quaternion_t q2)
{
    q1.w -= q2.w;
    q1.i -= q2.i;
    q1.j -= q2.j;
    q1.k -= q2.k;

    return q1;
}

quaternion_t QuatMultiply(quaternion_t q, float a)
{
    q.w *= a;
    q.i *= a;
    q.j *= a;
    q.k *= a;

    return q;
}

quaternion_t QuatProd(quaternion_t q1, quaternion_t q2)
{
    quaternion_t q;

    q.w = q1.w*q2.w-VectorDotProd(q1.v,q2.v);

    q.v = VectorSum(VectorMultiply(q2.v,q1.w),VectorSum(VectorMultiply(q1.v,q2.w),VectorCrossProd(q1.v,q2.v)));

    return q;
}

quaternion_t QuatInv(quaternion_t q)
{
    float squareSum = q.w*q.w + q.i*q.i + q.j*q.j + q.k*q.k;

    q.v = VectorNegative(q.v);

    if(squareSum == 0)
    {
        return q;
    }

    q.v = VectorMultiply(q.v,1/squareSum);
    q.w /= squareSum;
    return q;
}

float QuatLength(quaternion_t q)
{
    return sqrt(q.w*q.w + q.i*q.i + q.j*q.j + q.k*q.k);
}

quaternion_t QuatNorm(quaternion_t q)
{
    float length = QuatLength(q);

    if(length == 0)
    {
        return q;
    }

    q.w /= length;
    q.i /= length;
    q.j /= length;
    q.k /= length;

    return q;
}

vector_t QuatTranslateToRotationVector(quaternion_t q)
{
    float w = q.w == 0 ? 0.000000001 : q.w;

    float angle = atan(VectorLength(q.v)/w)*2;
    return VectorMultiply(VectorNorm(q.v),angle);
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

