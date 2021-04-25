/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/radio/radio.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date Apr 25, 2021
 ****************************************************************************/

#include "drivers/radio/radio.h"
#include "drivers/uart/uart.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static uint32_t dataInternal[4] = {0,0,0,0};

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

void RadioIrq(uint32_t* data /*radioChannel_t channel, bool risingEdge*/)
{
    memcpy(dataInternal,data,4*sizeof(uint32_t));
}

void RadioProcess()
{
    UartWrite("%i\t%i\t%i\t%i\r\n",dataInternal[0],dataInternal[1],dataInternal[2],dataInternal[3]);
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

