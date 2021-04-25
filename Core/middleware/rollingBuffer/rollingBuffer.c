/*****************************************************************************
 * @file /CALMAR/Core/middleware/rollingBuffer/rollingBuffer.c
 *
 * @brief 
 * 
 * @author Michal Frankiewicz
 * @date May 12, 2020
 *
 ****************************************************************************/

#include "middleware/rollingBuffer/rollingBuffer.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef struct{
    uint32_t elementSize;
    uint32_t bufferSize;
    uint32_t rollingIndex;
    uint8_t* data;
}rollingBufferData_t;

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

rollingBufferState_t RollingBufferCreateBuffer(rollingBufferHandle_t* bufferHandle, uint32_t elementSize, uint32_t bufferSize, void* initElementValue)
{
    if(bufferSize == 0 || elementSize == 0)
    {
        return RB_INVALID_SIZE_ERROR;
    }

    *bufferHandle = (rollingBufferHandle_t)malloc(sizeof(rollingBufferData_t));

    ((rollingBufferData_t*)*bufferHandle)->bufferSize = bufferSize;
    ((rollingBufferData_t*)*bufferHandle)->elementSize = elementSize;
    ((rollingBufferData_t*)*bufferHandle)->rollingIndex = 0;
    ((rollingBufferData_t*)*bufferHandle)->data = malloc(bufferSize*elementSize);



    for(uint32_t i=0; i<bufferSize;i++)
    {
        memcpy((uint8_t*)(((rollingBufferData_t*)*bufferHandle)->data+i*elementSize),initElementValue,elementSize);
    }

    return RB_OK;
}

rollingBufferState_t RollingBufferDestroyBuffer(rollingBufferHandle_t bufferHandle)
{
    if((rollingBufferData_t*)bufferHandle == NULL)
    {
        return RB_NULL_PTR_ERROR;
    }

    free(((rollingBufferData_t*)bufferHandle)->data);
    free(((rollingBufferData_t*)bufferHandle));
    return RB_OK;
}

rollingBufferState_t RollingBufferWrite(rollingBufferHandle_t bufferHandle, void* value)
{
    if((rollingBufferData_t*)bufferHandle == NULL || value == NULL)
    {
        return RB_NULL_PTR_ERROR;
    }
    rollingBufferData_t* bufferData = (rollingBufferData_t*)bufferHandle;

    bufferData->rollingIndex++;
    if(bufferData->rollingIndex >= bufferData->bufferSize)
    {
        bufferData->rollingIndex = 0;
    }

    uint8_t* address = (uint8_t*)(bufferData->data+bufferData->rollingIndex*bufferData->elementSize);
    memcpy(address,value,bufferData->elementSize);
    return RB_OK;
}

rollingBufferState_t RollingBufferRead(rollingBufferHandle_t bufferHandle, void* value, uint32_t index)
{
    if((rollingBufferData_t*)bufferHandle == NULL || value == NULL)
    {
        return RB_NULL_PTR_ERROR;
    }

    rollingBufferData_t* bufferData = (rollingBufferData_t*)bufferHandle;

    if(index >= bufferData->bufferSize)
    {
         return RB_INDEX_OUT_OF_BOUNDS_ERROR;
    }

    uint32_t ind;
    if(index > bufferData->rollingIndex)
    {
        ind = bufferData->bufferSize-(index-bufferData->rollingIndex);
    } else {
        ind = bufferData->rollingIndex-index;
    }
    uint8_t* address = (uint8_t*)(bufferData->data+ind*bufferData->elementSize);
    memcpy(value,address,bufferData->elementSize);
    return RB_OK;
}

rollingBufferState_t RollingBufferBurstRead(rollingBufferHandle_t bufferHandle, void* valueArray, uint32_t lastElements)
{
    if((rollingBufferData_t*)bufferHandle == NULL || valueArray == NULL)
    {
        return RB_NULL_PTR_ERROR;
    }

    rollingBufferData_t* bufferData = (rollingBufferData_t*)bufferHandle;

    if(lastElements > bufferData->bufferSize || lastElements == 0)
    {
        return RB_INDEX_OUT_OF_BOUNDS_ERROR;
    }

    uint32_t ind;
    if(lastElements-1 > bufferData->rollingIndex)
    {
        ind = bufferData->bufferSize-(lastElements-1-bufferData->rollingIndex);
        uint8_t* address = (uint8_t*)(bufferData->data+ind*bufferData->elementSize);
        uint32_t inds = (bufferData->bufferSize-ind);
        memcpy(valueArray,address,inds*bufferData->elementSize);
        memcpy(valueArray+inds*bufferData->elementSize,bufferData->data,(lastElements-inds)*bufferData->elementSize);

    } else {
        ind = bufferData->rollingIndex-(lastElements-1);
        uint8_t* address = (uint8_t*)(bufferData->data+ind*bufferData->elementSize);
        memcpy(valueArray,address,lastElements*bufferData->elementSize);
    }

    return RB_OK;
}

uint32_t RollingBufferGetBufferSize(rollingBufferHandle_t bufferHandle)
{
    rollingBufferData_t* bufferData = (rollingBufferData_t*)bufferHandle;
    return bufferData->bufferSize;
}
