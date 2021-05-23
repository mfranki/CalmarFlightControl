/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/digitalFilter/digitalFilter.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date 2 maj 2021
 ****************************************************************************/
#pragma once 

#include "middleware/rollingBuffer/rollingBuffer.h"

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef uint32_t digitalFilterHandle_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief allocates memory for filter and initializes it
 *        filter structure:
 *            U[k]*num[0]+U[k-1]*num[1] ... U[k-n]*num[n]
 *        H = -------------------------------------------
 *            Y[k]*den[0]+Y[k-1]*den[1] ... Y[k-n]*den[n]
 *
 *          where:
 *          U - filter input
 *          Y - filter output
 *          H - filter transmitance
 *
 * @param [in] numerator - cannot be NULL, needs to be an array the same size as denominator
 * @param [in] denominator - cannot be NULL, needs to be an array the same size as numerator
 * @param [in] filterOrder - equal to length of numerator and denominator - 1
 * @param [out] filterHandle - needed to access filter
 * @return true if successful
 */
bool DigitalFilterCreateFilter(float numerator[], float denominator[], uint32_t filterOrder, digitalFilterHandle_t* filterHandle);

/**@brief deinits filter and fres memory
 *
 * @param [in] filterHandle
 * @return true if successful
 */
bool DigitalFilterDeleteFilter(digitalFilterHandle_t filterHandle);

/**@brief filters given data, needs to be called in regular time intervals so the filter works correctly
 *
 * @param [in] filterHandle - needed to access the filter
 * @param [in] curentSignalValue - data to be filtered
 * @param [out] filterOutput - filtered data
 * @return true if successful
 */
bool DigitalFilterProcess(digitalFilterHandle_t filterHandle, float curentSignalValue, float* filterOutput);
