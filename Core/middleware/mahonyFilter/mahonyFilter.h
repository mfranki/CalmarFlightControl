/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/mahonyFilter/mahonyFilter.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "middleware/vector/vector.h"
#include "middleware/quaternion/quaternion.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief mahony filter task
 */
void MahonyFilterTask();

/**@brief sets reference position mag field vector,
 *        calculates mag vector part perpendicular to current acc initial vector
 *        and sets it as reference mag vector
 * @param [in] magVector
 */
void MahonyFilterSetInitialMagVector(vector_t magVector);

/**@brief sets acc reference vector
 *
 * @param [in] accVector
 */
void MahonyFilterSetInitialAccVector(vector_t accVector);

/**@brief enables magnetometer data to be used in filter measurements
 *
 * @param [in] useMag - if true mag data is used in filter calculations
 */
void MahonyFilterUseMagnetometer(bool useMag);

/**@brief getter for filtered orientation
 *
 * @return position
 */
quaternion_t MahonyFilterGetOrientation();

