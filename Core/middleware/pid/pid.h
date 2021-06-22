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

typedef enum{
    PID_P=0,
    PID_I,
    PID_D,
    PID_N,
}pidParameters_t;

typedef uint32_t pidHandle_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes discrete PID
 *
 * @param [in] pidHandle
 * @param [in] p
 * @param [in] i
 * @param [in] d
 * @param [in] filterCoefficient - for velocity approximation
 * @return true if successful
 */
bool PidInit(pidHandle_t *pidHandle, float p, float i, float d, float filterCoefficient);

/**@brief calculates next PID output value
 *        needs to be called regularly with desired sampling frequency
 *
 *         pid model:
 *
 *         P + I*Ts/(z-1) + D*N/(1+N*Ts/(z-1))
 *
 *         where this is velocity filter:
 *
 *         N/(1+N*Ts/(z-1))
 *
 * @param [in] pidHandle
 * @param [in] input input data
 * @return PID output
 */
float PidCalc(pidHandle_t pidHandle, float input);

/**@brief setter for given pid parameter
 *
 * @param [in] pidHandle
 * @param [in] param
 * @param [in] value
 * @return true if successful
 */
bool PidSetParam(pidHandle_t pidHandle, pidParameters_t param, float value);
