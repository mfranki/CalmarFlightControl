/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/vector/vector.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 1, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef struct{
    float x;
    float y;
    float z;
}vector_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief adds two vectors
 *
 * @param [in] v1
 * @param [in] v2
 * @return v1+v2
 */
vector_t VectorSum(vector_t v1, vector_t v2);

/**@brief subtract two vectors
 *
 * @param [in] v1
 * @param [in] v2
 * @return v1-v2
 */
vector_t VectorDiff(vector_t v1, vector_t v2);

/**@brief makes vector opposite sign
 *
 * @param [in]v
 * @return -v
 */
vector_t VectorNegative(vector_t v);

/**@brief multiplies vector by scalar
 *
 * @param [in]v
 * @param [in]a
 * @return v*a
 */
vector_t VectorMultiply(vector_t v, float a);

/**@brief performs dot product on two vectors
 *
 * @param [in]v1
 * @param [in]v2
 * @return v1*v2
 */
float VectorDotProd(vector_t v1, vector_t v2);

/**@brief performs cross product on two vectors
 *
 * @param [in]v1
 * @param [in]v2
 * @return v1 x v2
 */
vector_t VectorCrossProd(vector_t v1, vector_t v2);

/**@brief  calculates vector length
 *
 * @param [in]v
 * @return vector length
 */
float VectorLength(vector_t v);

/**@brief normalizes vector
 *
 * @param [in]v
 * @return v/|v|, in case |v|==0 returns v
 */
vector_t VectorNorm(vector_t v);
