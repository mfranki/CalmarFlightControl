/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/digitalFilter/digitalFilter.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date 2 maj 2021
 ****************************************************************************/

#include "middleware/digitalFilter/digitalFilter.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

typedef struct {
    float* numerator;
    float* denominator;
    uint32_t order;
    rollingBufferHandle_t bufferHandle;
}filter_t;

typedef struct{
    float output;
    float input;
}filterData_t;

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/



/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool DigitalFilterCreateFilter(float numerator[], float denominator[], uint32_t filterOrder, digitalFilterHandle_t* filterHandle)
{
    if(numerator == NULL ||
       denominator == NULL ||
       filterOrder == 0 ||
       filterHandle == NULL)
    {
        return false;
    }

    if(denominator[0] == 0)
    {
        return false;
    }

    *filterHandle = (digitalFilterHandle_t)malloc(sizeof(filter_t));
    if(*filterHandle == 0)
    {
        return false;
    }

    ((filter_t*)*filterHandle)->numerator = malloc(sizeof(float)*(filterOrder+1));

    if(((filter_t*)*filterHandle)->numerator == NULL)
    {
        free((filter_t*)*filterHandle);
        return false;
    }

    ((filter_t*)*filterHandle)->denominator = malloc(sizeof(float)*(filterOrder+1));
    if(((filter_t*)*filterHandle)->denominator == NULL)
    {
        free(((filter_t*)*filterHandle)->numerator);
        free((filter_t*)*filterHandle);
        return false;
    }

    filterData_t initialFilterData = {0,0};
    if(RB_OK != RollingBufferCreateBuffer(&(((filter_t*)*filterHandle)->bufferHandle),sizeof(filterData_t), filterOrder, &initialFilterData))
    {
        return false;
    }

    memcpy(((filter_t*)*filterHandle)->numerator, numerator, (filterOrder+1)*sizeof(float));
    memcpy(((filter_t*)*filterHandle)->denominator, denominator, (filterOrder+1)*sizeof(float));

    ((filter_t*)*filterHandle)->order = filterOrder;

    return true;
}

bool DigitalFilterDeleteFilter(digitalFilterHandle_t filterHandle)
{
    free(((filter_t*)filterHandle)->denominator);
    free(((filter_t*)filterHandle)->numerator);
    RollingBufferDestroyBuffer(((filter_t*)filterHandle)->bufferHandle);
    free((filter_t*)filterHandle);

    return true;
}

bool DigitalFilterProcess(digitalFilterHandle_t filterHandle, float curentSignalValue, float* filterOutput)
{
    *filterOutput = 0;

    for(uint32_t i=0; i<((filter_t*)filterHandle)->order; i++)
    {
        filterData_t fd;
        if(RB_OK != RollingBufferRead(((filter_t*)filterHandle)->bufferHandle, &fd, i))
        {
            return false;
        }

        /**< U(k-i-1)*numerator[i+1] - Y(k-i-1)*denominator[i+1] **/
        *filterOutput += fd.input*(((filter_t*)filterHandle)->numerator[i+1]) - fd.output*(((filter_t*)filterHandle)->denominator[i+1]);
    }

    /**< U(0)*numerator[0]**/
    *filterOutput += curentSignalValue*(((filter_t*)filterHandle)->numerator[0]);
    *filterOutput /= (((filter_t*)filterHandle)->denominator[0]);

    filterData_t newestData = {
            .input = curentSignalValue,
            .output = *filterOutput
    };

    if(RB_OK != RollingBufferWrite(((filter_t*)filterHandle)->bufferHandle,&newestData))
    {
        return false;
    }

    return true;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

