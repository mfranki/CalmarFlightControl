/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/BMX055/BMX055.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date Jan 3, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

typedef struct{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float mx;
    float my;
    float mz;
}bmx055Data_t;

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

bool Bmx055Init(SPI_HandleTypeDef *HSPI);

bool BMX055CalibrateAccGyro();

bool BMX055CalibrateMag();

bool Bmx055GetData(bmx055Data_t* data);
