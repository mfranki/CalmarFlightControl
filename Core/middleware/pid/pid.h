/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/pid/pid.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 2, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef void* pidHandle_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes discrete PID
 *
 * @param [in] p
 * @param [in] i
 * @param [in] d
 * @param [in] filterCoefficient - for velocity approximation
 * @return handle to pid regulator
 */
pidHandle_t PidInit(float p, float i, float d, float filterCoefficient);

/**@brief calculates next PID output value
 *        needs to be called regularly with desired sampling frequency
 *
 * @param [in] pidHandle
 * @param [in] input input data
 * @return PID output
 */
float PidCalc(pidHandle_t pidHandle, float input);

/**@brief calculates PID using also externally calculated velocity
 *
 * @param [in] pidHandle
 * @param [in] input - input data
 * @param [in] inputVel - externally calculated input data velocity
 * @return PID output
 */
float PidCalcExternVel(pidHandle_t pidHandle, float input, float inputVel);
