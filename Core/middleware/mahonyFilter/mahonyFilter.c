/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/mahonyFilter/mahonyFilter.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date May 1, 2021
 ****************************************************************************/

#include "middleware/mahonyFilter/mahonyFilter.h"
#include "middleware/digitalFilter/digitalFilter.h"
#include "drivers/BMX055/BMX055.h"
#include "drivers/uart/uart.h"

#include "cmsis_os.h"
#include <main.h>
#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/



/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef struct {
    float x;
    float y;
    float z;
}vector_t;

digitalFilterHandle_t accFilterHandle[3];

digitalFilterHandle_t magFilterHandle[3];

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/



/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool MahonyFilterInit()
{
    bool success = true;
    /**25Hz low pass filter, sampling 1kHz **/
    float accNumerator[] = {0.00554271721028068,0.0110854344205614,0.00554271721028068};
    float accDenominator[] = {1,-1.77863177782459,0.800802646665708};
    success &= DigitalFilterCreateFilter(accNumerator, accDenominator, 2, &accFilterHandle[0]);    ///< x
    success &= DigitalFilterCreateFilter(accNumerator, accDenominator, 2, &accFilterHandle[1]);    ///< y
    success &= DigitalFilterCreateFilter(accNumerator, accDenominator, 2, &accFilterHandle[2]);    ///< z

    /**20Hz low pass filter, reading samples 1kHz, sampling ~=30Hz **/
    float magNumerator[] = {0.00362168151492864,0.00724336302985729,0.00362168151492864};
    float magDenominator[] = {1,-1.82269492519631,0.837181651256023};
    success &= DigitalFilterCreateFilter(magNumerator, magDenominator, 2, &magFilterHandle[0]);    ///< x
    success &= DigitalFilterCreateFilter(magNumerator, magDenominator, 2, &magFilterHandle[1]);    ///< y
    success &= DigitalFilterCreateFilter(magNumerator, magDenominator, 2, &magFilterHandle[2]);    ///< z

   return success;
}

void MahonyFilterTask()
{
    bmx055Data_t data;

    TickType_t lastTickTime = xTaskGetTickCount();
    float posY = 0;

    while(1)
    {
        HAL_GPIO_WritePin(DEBUG_OUT_1_GPIO_Port,DEBUG_OUT_1_Pin,1);
        Bmx055GetData(&data);


        posY += data.gy*0.001;
        //UartWrite("%f\t%f\t%f\r\n",data.gy,posY);
//        UartWrite("%f\t%f\t%f\r\n",data.gx,data.gy,data.gz);


        bmx055Data_t filteredData;
        DigitalFilterProcess(accFilterHandle[0], data.ax, &filteredData.ax);
        DigitalFilterProcess(accFilterHandle[1], data.ay, &filteredData.ay);
        DigitalFilterProcess(accFilterHandle[2], data.az, &filteredData.az);

        DigitalFilterProcess(magFilterHandle[0], data.mx, &filteredData.mx);
        DigitalFilterProcess(magFilterHandle[1], data.my, &filteredData.my);
        DigitalFilterProcess(magFilterHandle[2], data.mz, &filteredData.mz);

        HAL_GPIO_WritePin(DEBUG_OUT_1_GPIO_Port,DEBUG_OUT_1_Pin,0);

        UartWrite("%f\t%f\t%f\r\n",data.mx,data.my,data.mz);

        vTaskDelayUntil(&lastTickTime,10);
    }
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

