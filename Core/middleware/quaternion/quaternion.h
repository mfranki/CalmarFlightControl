/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/quaternion/quaternion.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 1, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "middleware/vector/vector.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef struct{
    float w;

    union{
        struct{
            float i;
            float j;
            float k;
        };
        vector_t v;
    };

}quaternion_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/** adds two quaternions
 *
 * @param [in]q1
 * @param [in]q2
 * @return q1+q2
 */
quaternion_t QuatSum(quaternion_t q1, quaternion_t q2);

/** subtracts two quaternions
 *
 * @param [in]q1
 * @param [in]q2
 * @return q1-q2
 */
quaternion_t QuatDiff(quaternion_t q1, quaternion_t q2);

/**multiplies quaternions by scalar
 *
 * @param [in]q
 * @param [in]a
 * @return q*a
 */
quaternion_t QuatMultiply(quaternion_t q, float a);

/**calculates quaternion product
 *
 * @param [in]q1
 * @param [in]q2
 * @return quaternion product
 */
quaternion_t QuatProd(quaternion_t q1, quaternion_t q2);

/**@brief inverts quaterinon
 *
 * @param [in]q
 * @return q^-1, if |q|==0 returns q
 */
quaternion_t QuatInv(quaternion_t q);

/**@brief calculates quaternion length
 *
 * @param [in]q
 * @return length
 */
float QuatLength(quaternion_t q);

/**@brief normalizes quaternion
 *
 * @param [in]q
 * @return ||q|| if |q|==0 returns q
 */
quaternion_t QuatNorm(quaternion_t q);

/**@brief translates quaternion to rotation described by rotation around vector v by angle = ||v||
 *
 * @param [in] q
 * @return rotation vector
 */
vector_t QuatTranslateToRotationVector(quaternion_t q);

/**@brief translates rotation described by rotation around vector v by angle = ||v|| to quaternion
 *
 * @param [in] v rotation vector
 * @return rotation quaternion
 */
quaternion_t QuatTranslateVectorToQuaternion(vector_t v);
