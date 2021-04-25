/*****************************************************************************
 * @file /CALMAR/Core/middleware/rollingBuffer/rollingBuffer.h
 *
 * @brief 
 * 
 * @author Michal Frankiewicz
 * @date May 12, 2020
 *
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

/**@brief ROLLING_BUFFER_STATE_MESSAGES
 */
typedef enum{
    RB_OK=1,                     //!< RB_OK
    RB_INVALID_SIZE_ERROR,       //!< RB_INVALID_SIZE_ERROR
    RB_INVALID_TYPE_ERROR,       //!< RB_INVALID_TYPE_ERROR
    RB_NULL_PTR_ERROR,           //!< RB_NULL_PTR_ERROR
    RB_INDEX_OUT_OF_BOUNDS_ERROR,//!< RB_INDEX_OUT_OF_BOUNDS_ERROR
}rollingBufferState_t;

/**@brief rolling buffer handle, identifies buffer
 */
typedef uint32_t rollingBufferHandle_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief allocates memory for buffer and, initializes its fields
 *
 * @param [out] bufferHandle - identifies buffer
 * @param [in] elementSize - sizeof buffer element must be >0
 * @param [in] bufferSize - must be >0
 * @param [in] initValue - pointer to initial buffer data (field, struct etc.) which is copied to all of buffer elements
 * @return operation state, RB_OK if no error @ref ROLLING_BUFFER_STATE_MESSAGES
 */
rollingBufferState_t RollingBufferCreateBuffer(rollingBufferHandle_t* bufferHandle, uint32_t elementSize, uint32_t bufferSize, void* initElementValue);

/**@brief frees memory occupied by buffer
 *
 * @param [in] bufferHandle - identifies buffer
 * @return operation state, RB_OK if no error @ref ROLLING_BUFFER_STATE_MESSAGES
 */
rollingBufferState_t RollingBufferDestroyBuffer(rollingBufferHandle_t bufferHandle);

/**@brief writes new data to the buffer
 *
 * @param [in] bufferHandle - identifies buffer
 * @param [in] value - pointer to data, field, struct etc.
 * @return operation state, RB_OK if no error @ref ROLLING_BUFFER_STATE_MESSAGES
 */
rollingBufferState_t RollingBufferWrite(rollingBufferHandle_t bufferHandle, void* value);

/**@brief read data from buffer
 *
 * @param [in] bufferHandle - identifies buffer
 * @param [out] value - pointer to data, field, struct etc.
 * @param [in] index - has to be <bufferSize, 0 means most recent element, bufferSize-1 means the oldest element
 * @return operation state, RB_OK if no error @ref ROLLING_BUFFER_STATE_MESSAGES
 */
rollingBufferState_t RollingBufferRead(rollingBufferHandle_t bufferHandle, void* value, uint32_t index);

/**@brief read last n elements from buffer,
 *        where most recent element is at the end of @param valueArray and the oldest one at the beginning
 *
 * @param [in] bufferHandle - identifies buffer
 * @param [out] valueArray - pointer to data array, field, struct etc.
 * @param [in] lastElements - has to be <bufferSize, 0 means most recent element, bufferSize-1 means all the elements
 * @return operation state, RB_OK if no error @ref ROLLING_BUFFER_STATE_MESSAGES
 */
rollingBufferState_t RollingBufferBurstRead(rollingBufferHandle_t bufferHandle, void* valueArray, uint32_t lastElements);

/**@brief returns buffer length
 *
 * @param [in] bufferHandle - identifies buffer
 * @return buffer length
 */
uint32_t RollingBufferGetBufferSize(rollingBufferHandle_t bufferHandle);
