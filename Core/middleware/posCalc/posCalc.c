/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/posCalc/posCalc.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date 18 sty 2021
 ****************************************************************************/

#include "middleware/posCalc/posCalc.h"
#include "drivers/BMX055/BMX055.h"
#include "drivers/uart/uart.h"
#include "middleware/rollingBuffer/rollingBuffer.h"

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define ACC_ROLL_SHARE (0.02f)
#define ACC_PITCH_SHARE (0.02f)

#define ACC_LOW_PASS_FILTER_BUFFER_SIZE (30U)
#define GYRO_LOW_PASS_FILTER_BUFFER_SIZE (10U)
#define MAG_LOW_PASS_FILTER_BUFFER_SIZE (20U)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static float roll = 0;
static float pitch = 0;
static float yaw;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

rollingBufferHandle_t bufferHandleX;
rollingBufferHandle_t bufferHandleY;
rollingBufferHandle_t bufferHandleZ;
float accSumX = 0;
float accSumY = 0;
float accSumZ = 0;

rollingBufferHandle_t bufferHandleXg;
rollingBufferHandle_t bufferHandleYg;
float gyroSumX = 0;
float gyroSumY = 0;

rollingBufferHandle_t bufferHandleXm;
rollingBufferHandle_t bufferHandleYm;
float magSumX = 0;
float magSumY = 0;

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool PosCalcInit()
{
    float zero = 0.0f;
    RollingBufferCreateBuffer(&bufferHandleX, sizeof(float), ACC_LOW_PASS_FILTER_BUFFER_SIZE, &zero);
    RollingBufferCreateBuffer(&bufferHandleY, sizeof(float), ACC_LOW_PASS_FILTER_BUFFER_SIZE, &zero);
    RollingBufferCreateBuffer(&bufferHandleZ, sizeof(float), ACC_LOW_PASS_FILTER_BUFFER_SIZE, &zero);

    RollingBufferCreateBuffer(&bufferHandleXg, sizeof(float), GYRO_LOW_PASS_FILTER_BUFFER_SIZE, &zero);
    RollingBufferCreateBuffer(&bufferHandleYg, sizeof(float), GYRO_LOW_PASS_FILTER_BUFFER_SIZE, &zero);

    RollingBufferCreateBuffer(&bufferHandleXm, sizeof(float), MAG_LOW_PASS_FILTER_BUFFER_SIZE, &zero);
    RollingBufferCreateBuffer(&bufferHandleYm, sizeof(float), MAG_LOW_PASS_FILTER_BUFFER_SIZE, &zero);

    bool BMX055CalibrateAccGyro();
    return true;
}
static uint32_t dataInternal[4] = {0,1,2,3};

bool PosCalcDispPos()
{
    bmx055Data_t sensorData;

    Bmx055GetData(&sensorData);
    //UartWrite("%i\t%i\t%i\t%i\r\n",dataInternal[0],dataInternal[1],dataInternal[2],dataInternal[3]);
    UartWrite("%f\t%f\t%f\r\n",sensorData.ay,sensorData.my,sensorData.gy);
    //UartWrite("%f\r\n",sqrt(sensorData.mx*sensorData.mx+sensorData.my*sensorData.my+sensorData.mz*sensorData.mz));
    ///acc filtering
    float value = 0;
    RollingBufferRead(bufferHandleX, &value, ACC_LOW_PASS_FILTER_BUFFER_SIZE-1);
    accSumX += sensorData.ax-value;
    RollingBufferWrite(bufferHandleX, &sensorData.ax);
    sensorData.ax = accSumX/ACC_LOW_PASS_FILTER_BUFFER_SIZE;

    RollingBufferRead(bufferHandleY, &value, ACC_LOW_PASS_FILTER_BUFFER_SIZE-1);
    accSumY += sensorData.ay-value;
    RollingBufferWrite(bufferHandleY, &sensorData.ay);
    sensorData.ay = accSumY/ACC_LOW_PASS_FILTER_BUFFER_SIZE;

    RollingBufferRead(bufferHandleZ, &value, ACC_LOW_PASS_FILTER_BUFFER_SIZE-1);
    accSumZ += sensorData.az-value;
    RollingBufferWrite(bufferHandleZ, &sensorData.az);
    sensorData.az = accSumZ/ACC_LOW_PASS_FILTER_BUFFER_SIZE;

    ///gyro filtering
    RollingBufferRead(bufferHandleXg, &value, GYRO_LOW_PASS_FILTER_BUFFER_SIZE-1);
    gyroSumX += sensorData.gx-value;
    RollingBufferWrite(bufferHandleXg, &sensorData.gx);
    sensorData.gx = gyroSumX/GYRO_LOW_PASS_FILTER_BUFFER_SIZE;

    RollingBufferRead(bufferHandleYg, &value, GYRO_LOW_PASS_FILTER_BUFFER_SIZE-1);
    gyroSumY += sensorData.gy-value;
    RollingBufferWrite(bufferHandleYg, &sensorData.gy);
    sensorData.gy = gyroSumY/GYRO_LOW_PASS_FILTER_BUFFER_SIZE;


    ///mag filtering
    RollingBufferRead(bufferHandleXm, &value, MAG_LOW_PASS_FILTER_BUFFER_SIZE-1);
    magSumX += sensorData.mx-value;
    RollingBufferWrite(bufferHandleXm, &sensorData.mx);
    sensorData.mx = magSumX/MAG_LOW_PASS_FILTER_BUFFER_SIZE;

    RollingBufferRead(bufferHandleYm, &value, MAG_LOW_PASS_FILTER_BUFFER_SIZE-1);
    magSumY += sensorData.my-value;
    RollingBufferWrite(bufferHandleYm, &sensorData.my);
    sensorData.my = magSumY/MAG_LOW_PASS_FILTER_BUFFER_SIZE;


    float nextRoll = roll + sensorData.gy*0.01;
    float nextPitch = pitch + sensorData.gz*0.01;
    roll = nextRoll*(1-ACC_ROLL_SHARE) + ACC_ROLL_SHARE*atan2(sensorData.ay,sensorData.az)*180/3.141;
    pitch = nextPitch*(1-ACC_PITCH_SHARE) + ACC_PITCH_SHARE*atan2(-sensorData.ax,sensorData.az)*180/3.141;

    //UartWrite("%f\t%f\t%f\r\n",roll,pitch,atan2(sensorData.my,sensorData.mx)*180/3.141);

    //UartWrite("%f\r\n",sensorData.ay);
    return true;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

