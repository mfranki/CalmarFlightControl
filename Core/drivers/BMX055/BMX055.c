/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/BMX055/BMX055.c
 *
 * @brief Source code file template
 * 
 * @author Michal Frankiewicz
 * @date Jan 3, 2021
 ****************************************************************************/

#include "drivers/BMX055/BMX055.h"

#include "drivers/uart/uart.h"
#include "drivers/utils/utils.h"
#include "drivers/eeprom/eeprom.h"

#include "middleware/memory/memory.h"

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/


/**BMX ADDRESSING DATA**/
#define ACC_MIN_ADDRESS (0x00U)
#define ACC_MAX_ADDRESS (0x3FU)
#define GYRO_MIN_ADDRESS (0x00U)
#define GYRO_MAX_ADDRESS (0x3EU)
#define MAG_MIN_ADDRESS (0x40U)
#define MAG_MAX_ADDRESS (0x52U)



/**ACC REGISTERS AND BITS**/
#define ACC_BGW_CHIPID (0x00U)  ///< REGISTER ADDRESS
#define ACC_ID_VALUE (0xFAU)

#define ACC_ACCD_X_LSB (0x02U)  ///< REGISTER ADDRESS
#define ACC_ACCD_X_LSB_ACC_X_LSB_POS (0x4U)
#define ACC_ACCD_X_LSB_ACC_X_LSB_MASK (0x0FU<<ACC_ACCD_X_LSB_ACC_X_LSB_POS)
#define ACC_ACCD_X_LSB_NEW_DATA_X_BIT (0x01U)

#define ACC_ACCD_X_MSB (0x03U)  ///< REGISTER ADDRESS

#define ACC_ACCD_Y_LSB (0x04U)  ///< REGISTER ADDRESS
#define ACC_ACCD_Y_LSB_ACC_Y_LSB_POS (0x4U)
#define ACC_ACCD_Y_LSB_ACC_Y_LSB_MASK (0x0FU<<ACC_ACCD_Y_LSB_ACC_Y_LSB_POS)
#define ACC_ACCD_Y_LSB_NEW_DATA_Y_BIT (0x01U)

#define ACC_ACCD_Y_MSB (0x05U)  ///< REGISTER ADDRESS

#define ACC_ACCD_Z_LSB (0x06U)  ///< REGISTER ADDRESS
#define ACC_ACCD_Z_LSB_ACC_Z_LSB_POS (0x4U)
#define ACC_ACCD_Z_LSB_ACC_Z_LSB_MASK (0x0FU<<ACC_ACCD_Z_LSB_ACC_Z_LSB_POS)
#define ACC_ACCD_Z_LSB_NEW_DATA_Z_BIT (0x01U)

#define ACC_ACCD_Z_MSB (0x07U)  ///< REGISTER ADDRESS

#define ACC_PMU_RANGE (0x0FU)   ///< REGISTER ADDRESS
#define ACC_PMU_RANGE_2G (0x03U)
#define ACC_PMU_RANGE_4G (0x05U)
#define ACC_PMU_RANGE_8G (0x08U)
#define ACC_PMU_RANGE_16G (0x0CU)
#define ACC_PMU_RESOLUTION_2G (0.00098f)    ///< accelerometer resolution in [g/LSB]
#define ACC_PMU_RESOLUTION_4G (0.00195f)    ///< accelerometer resolution in [g/LSB]
#define ACC_PMU_RESOLUTION_8G (0.00391f)    ///< accelerometer resolution in [g/LSB]
#define ACC_PMU_RESOLUTION_16G (0.00781f)    ///< accelerometer resolution in [g/LSB]
#define ACC_PMU_DEFAULT_RANGE ACC_PMU_RANGE_8G

#define ACC_ACCD_HBW (0x13U)    ///< REGISTER ADDRESS
#define ACC_ACCD_HBW_FILTER_DISABLE_BIT (0x80U)
#define ACC_ACCD_HBW_SHADOW_DISABLE_BIT (0x40U)

#define ACC_BGW_SOFTRESET (0x14U)   ///< REGISTER ADDRESS
#define ACC_BGW_SOFTRESET_CODE (0xB6U)

#define ACC_OFC_CTRL (0x36U)    ///< REGISTER ADDRESS
#define ACC_OFC_CTRL_OFFSET_RESET_BIT (0x36U)
#define ACC_OFC_CTRL_FAST_COMPENSATION_X (0x20U)
#define ACC_OFC_CTRL_FAST_COMPENSATION_Y (0x40U)
#define ACC_OFC_CTRL_FAST_COMPENSATION_Z (0x60U)
#define ACC_OFC_CTRL_CAL_READY_BIT (0x10U)
#define ACC_OFC_CTRL_SLOW_COMPENSATION_X (0x01U)
#define ACC_OFC_CTRL_SLOW_COMPENSATION_Y (0x02U)
#define ACC_OFC_CTRL_SLOW_COMPENSATION_Z (0x04U)

#define ACC_OFC_SETTING (0x37U) ///< REGISTER ADDRESS
#define ACC_OFC_SETTING_OFFSET_TARGET_Z_0G (0x00U<<5U)
#define ACC_OFC_SETTING_OFFSET_TARGET_Z_PLUS_1G (0x01U<<5U)
#define ACC_OFC_SETTING_OFFSET_TARGET_Z_MINUS_1G (0x02U<<5U)
#define ACC_OFC_SETTING_OFFSET_TARGET_Y_0G (0x00U<<3U)
#define ACC_OFC_SETTING_OFFSET_TARGET_Y_PLUS_1G (0x01U<<3U)
#define ACC_OFC_SETTING_OFFSET_TARGET_Y_MINUS_1G (0x02U<<3U)
#define ACC_OFC_SETTING_OFFSET_TARGET_X_0G (0x00U<<1U)
#define ACC_OFC_SETTING_OFFSET_TARGET_X_PLUS_1G (0x01U<<1U)
#define ACC_OFC_SETTING_OFFSET_TARGET_X_MINUS_1G (0x02U<<1U)
#define ACC_OFC_SETTING_CUTOFF_FREQUENCY_BIT (0x01U)

#define ACC_OFC_OFFSET_X (0x38U)    ///< REGISTER ADDRESS

#define ACC_OFC_OFFSET_Y (0x39U)    ///< REGISTER ADDRESS

#define ACC_OFC_OFFSET_Z (0x3AU)    ///< REGISTER ADDRESS



/**GYRO REGISTERS AND BITS**/
#define GYRO_CHIP_ID (0x00U)    ///< REGISTER ADDRESS
#define GYRO_ID (0x0FU)

#define GYRO_RATE_X_LSB (0x01U)    ///< REGISTER ADDRESS

#define GYRO_RATE_X_MSB (0x02U)    ///< REGISTER ADDRESS

#define GYRO_RATE_Y_LSB (0x03U)    ///< REGISTER ADDRESS

#define GYRO_RATE_Y_MSB (0x04U)    ///< REGISTER ADDRESS

#define GYRO_RATE_Z_LSB (0x05U)    ///< REGISTER ADDRESS

#define GYRO_RATE_Z_MSB (0x06U)    ///< REGISTER ADDRESS

#define GYRO_RANGE (0x0FU)    ///< REGISTER ADDRESS
#define GYRO_RANGE_125_DEG (0x04U)
#define GYRO_RANGE_250_DEG (0x03U)
#define GYRO_RANGE_500_DEG (0x02U)
#define GYRO_RANGE_1000_DEG (0x01U)
#define GYRO_RANGE_2000_DEG (0x00U)
#define GYRO_RESOLUTION_125_DEG (0.0038f)   ///< gyro resolution in [(deg/s)/LSB
#define GYRO_RESOLUTION_250_DEG (0.0076f)   ///< gyro resolution in [(deg/s)/LSB
#define GYRO_RESOLUTION_500_DEG (0.0153f)   ///< gyro resolution in [(deg/s)/LSB
#define GYRO_RESOLUTION_1000_DEG (0.0305f)   ///< gyro resolution in [(deg/s)/LSB
#define GYRO_RESOLUTION_2000_DEG (0.061f)   ///< gyro resolution in [(deg/s)/LSB
#define GYRO_RANGE_DEFAULT GYRO_RANGE_250_DEG

#define GYRO_HBW (0x13U)    ///< REGISTER ADDRESS
#define GYRO_HBW_SHADOW_DISABLE_BIT (0x40U)
#define GYRO_HBW_FILTER_DISABLE_BIT (0x80U)

#define GYRO_FOC (0x32U)    ///< REGISTER ADDRESS
#define GYRO_FOC_FAST_OFFSET_EN_X_BIT (0x01U)
#define GYRO_FOC_FAST_OFFSET_EN_Y_BIT (0x02U)
#define GYRO_FOC_FAST_OFFSET_EN_Z_BIT (0x04U)
#define GYRO_FOC_FAST_OFFSET_EN_BIT (0x08U)
#define GYRO_FOC_FAST_OFFSET_WORDLENGTH_32_SAMP (0x00U)
#define GYRO_FOC_FAST_OFFSET_WORDLENGTH_64_SAMP (0x10U)
#define GYRO_FOC_FAST_OFFSET_WORDLENGTH_128_SAMP (0x20U)
#define GYRO_FOC_FAST_OFFSET_WORDLENGTH_256_SAMP (0x30U)
#define GYRO_FOC_AUTO_OFFSET_WORDLENGTH_32_SAMP (0x00U)
#define GYRO_FOC_AUTO_OFFSET_WORDLENGTH_64_SAMP (0x40U)
#define GYRO_FOC_AUTO_OFFSET_WORDLENGTH_128_SAMP (0x80U)
#define GYRO_FOC_AUTO_OFFSET_WORDLENGTH_256_SAMP (0xC0U)



/**MAG REGISTERS AND BITS**/
#define MAG_CHIP_ID (0x40U)    ///< REGISTER ADDRESS
#define MAG_ID (0x32U)

#define MAG_DATA_X_LSB (0x42U)    ///< REGISTER ADDRESS
#define MAG_DATA_X_LSB_DATAX_LSB_POS (0x03U)
#define MAG_DATA_X_LSB_SELF_TEST_BIT (0x01U)

#define MAG_DATA_X_MSB (0x43U)    ///< REGISTER ADDRESS

#define MAG_DATA_Y_LSB (0x44U)    ///< REGISTER ADDRESS
#define MAG_DATA_Y_LSB_DATAY_LSB_POS (0x03U)
#define MAG_DATA_Y_LSB_SELF_TEST_BIT (0x01U)

#define MAG_DATA_Y_MSB (0x45U)    ///< REGISTER ADDRESS

#define MAG_DATA_Z_LSB (0x46U)    ///< REGISTER ADDRESS
#define MAG_DATA_Z_LSB_DATAZ_LSB_POS (0x01U)
#define MAG_DATA_Z_LSB_SELF_TEST_BIT (0x01U)

#define MAG_DATA_Z_MSB (0x47U)    ///< REGISTER ADDRESS

#define MAG_RESOLUTION (0.0000625f) ///< mag resolution in [T/LSB]

#define MAG_POWER_CONTROL (0x4BU)    ///< REGISTER ADDRESS
#define MAG_POWER_CONTROL_SOFT_RESET_BIT1 (0x80U)
#define MAG_POWER_CONTROL_SPI3EN_BIT (0x04U)
#define MAG_POWER_CONTROL_SOFT_RESET_BIT2 (0x02U)
#define MAG_POWER_CONTROL_POWER_CONTROL_BIT (0x01U)

#define MAG_OPMODE (0x4CU)    ///< REGISTER ADDRESS
#define MAG_OPMODE_ADVANCED_SELFTEST_NEGATIVE_FIELD (0x80U)
#define MAG_OPMODE_ADVANCED_SELFTEST_POSITIVE_FIELD (0xC0U)
#define MAG_OPMODE_DATARATE_10HZ (0x00U<<3)
#define MAG_OPMODE_DATARATE_2HZ (0x01U<<3)
#define MAG_OPMODE_DATARATE_6HZ (0x02U<<3)
#define MAG_OPMODE_DATARATE_8HZ (0x03U<<3)
#define MAG_OPMODE_DATARATE_15HZ (0x04U<<3)
#define MAG_OPMODE_DATARATE_20HZ (0x05U<<3)
#define MAG_OPMODE_DATARATE_25HZ (0x06U<<3)
#define MAG_OPMODE_DATARATE_30HZ (0x07U<<3)
#define MAG_OPMODE_NORMAL (0x00U<<1)
#define MAG_OPMODE_FORCED (0x01U<<1)
#define MAG_OPMODE_SLEEP (0x03U<<1)
#define MAG_OPMODE_NORMAL_SELFTEST_BIT (0x01U)

#define MAG_ITR_AX_EN (0x4EU)    ///< REGISTER ADDRESS
#define MAG_ITR_AX_EN_DRDY_PIN_EN (0x01U<<7)
#define MAG_ITR_AX_EN_ITR_PIN_EN (0x01U<<6)
#define MAG_ITR_AX_EN_CHAN_Z_DIS (0x01U<<5)
#define MAG_ITR_AX_EN_CHAN_Y_DIS (0x01U<<4)
#define MAG_ITR_AX_EN_CHAN_X_DIS (0x01U<<3)
#define MAG_ITR_AX_EN_DRDY_POLARITY (0x01U<<2)
#define MAG_ITR_AX_EN_ITR_LATCH (0x01U<<1)
#define MAG_ITR_AX_EN_ITR_POLARITY (0x01U)

#define MAG_REP_XY (0x51U)    ///< REGISTER ADDRESS

#define MAG_REP_Z (0x52U)    ///< REGISTER ADDRESS

#define EARTH_GRAVITY_ACC (9.81f)

/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

typedef enum{
    ACC=0,      //!< ACC
    GYRO,       //!< GYRO
    MAG,        //!< MAG
    MODULE_COUNT//!< MODULE_COUNT
}imuModules_t;

typedef struct{
    uint32_t csPin;
    GPIO_TypeDef* csPort;
    uint8_t minAddress;
    uint8_t maxAddress;
}bmxParam_t;

static bmxParam_t bmxParams[MODULE_COUNT] = {
        {CS_ACC_Pin,CS_ACC_GPIO_Port,ACC_MIN_ADDRESS,ACC_MAX_ADDRESS},
        {CS_GYRO_Pin,CS_GYRO_GPIO_Port,GYRO_MIN_ADDRESS,GYRO_MAX_ADDRESS},
        {CS_MAG_Pin,CS_MAG_GPIO_Port,MAG_MIN_ADDRESS,MAG_MAX_ADDRESS}};


static float accXOffset = 0;
static float accYOffset = 0;
static float accZOffset = 0;

static float gyroXOffset = 0;
static float gyroYOffset = 0;
static float gyroZOffset = 0;

static float magXOffset = 0;
static float magYOffset = 0;
static float magZOffset = 0;
static float magXScale = 1;
static float magYScale = 1;
static float magZScale = 1;

static float accResolution = ACC_PMU_RESOLUTION_2G;
static float gyroResolution =  GYRO_RESOLUTION_2000_DEG;
static float magResolution = 0.3;    ///< [uT]

SPI_HandleTypeDef *hspi;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

static bool ReadAddress(imuModules_t module, uint8_t address, uint8_t* data);
static bool WriteAddress(imuModules_t module, uint8_t address, uint8_t data);
static bool ReadBurst(imuModules_t module, uint8_t address, uint8_t* data, uint8_t size);

static bool SetAccRange(uint8_t range);
static bool SetGyroRange(uint8_t range);

static bool CheckConnection();

static bool NvmInit();

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool Bmx055Init(SPI_HandleTypeDef *HSPI)
{
    if(HSPI == NULL)
    {
        return false;
    }
    hspi = HSPI;
    HAL_GPIO_WritePin(CS_ACC_GPIO_Port,CS_ACC_Pin,1);
    HAL_GPIO_WritePin(CS_GYRO_GPIO_Port,CS_GYRO_Pin,1);
    HAL_GPIO_WritePin(CS_MAG_GPIO_Port,CS_MAG_Pin,1);

    if(!CheckConnection())
    {
        return false;
    }

    /**SETUP ACC **/
    if(!WriteAddress(ACC, ACC_ACCD_HBW, ACC_ACCD_HBW_FILTER_DISABLE_BIT))
    {
        return false;
    }

    if(!WriteAddress(ACC, ACC_OFC_SETTING, ACC_OFC_SETTING_OFFSET_TARGET_Z_PLUS_1G |
                                           ACC_OFC_SETTING_OFFSET_TARGET_Y_0G |
                                           ACC_OFC_SETTING_OFFSET_TARGET_X_0G))
    {
        return false;
    }

    if(!SetAccRange(ACC_PMU_DEFAULT_RANGE))
    {
        return false;
    }

    /**SETUP GYRO**/
    if(!WriteAddress(GYRO, GYRO_HBW, GYRO_HBW_FILTER_DISABLE_BIT))
    {
        return false;
    }

    if(!SetGyroRange(GYRO_RANGE_DEFAULT))
    {
        return false;
    }

    /** SETUP MAG **/

    if(!WriteAddress(MAG, MAG_OPMODE, MAG_OPMODE_DATARATE_30HZ |
                                      MAG_OPMODE_NORMAL))
    {
        return false;
    }

    if(!WriteAddress(MAG, MAG_ITR_AX_EN, MAG_ITR_AX_EN_DRDY_PIN_EN |
                                         MAG_ITR_AX_EN_DRDY_POLARITY))
    {
        return false;
    }

    if(!WriteAddress(MAG, MAG_REP_XY, 0x10U))   ///< max repetitions 1+2*0xFF = 511
    {
        return false;
    }

    if(!WriteAddress(MAG, MAG_REP_Z, 0x10U))    ///< max repetitions 1+0xFF = 256
    {
        return false;
    }

    if(!NvmInit())
    {
        return false;
    }
/*

    uint8_t data[0x3E];
    for(uint8_t address=0x00,i=0; address<0x3E; address++,i++)
    {
        ReadAddress(GYRO, address, &data[i]);
    }
*/
    return true;
}

bool Bmx055GetData(bmx055Data_t* data)
{

    static uint8_t accRaw[6];  ///< x, y, z: lsb, msb = 3*2=6 bytes

    if(!ReadBurst(ACC, ACC_ACCD_X_LSB, accRaw, 6))
    {
        return false;
    }

    /**combine bits together**/
    int16_t axRaw = ((int16_t) accRaw[3])<<4 | ((int16_t) accRaw[2])>>4;
    int16_t ayRaw = ((int16_t) accRaw[1])<<4 | ((int16_t) accRaw[0])>>4;
    int16_t azRaw = ((int16_t) accRaw[5])<<4 | ((int16_t) accRaw[4])>>4;
    data->ax = (-(float)((axRaw&0x7ff)-(axRaw&0x800))*accResolution)*EARTH_GRAVITY_ACC-accXOffset;
    data->ay = ((float)((ayRaw&0x7ff)-(ayRaw&0x800))*accResolution)*EARTH_GRAVITY_ACC-accYOffset;
    data->az = (-(float)((azRaw&0x7ff)-(azRaw&0x800))*accResolution)*EARTH_GRAVITY_ACC-accZOffset;

    static uint8_t gyroRaw[6]; ///< x, y, z: lsb, msb = 3*2=6 bytes

    if(!ReadBurst(GYRO, GYRO_RATE_X_LSB, gyroRaw, 6))
    {
        return false;
    }
    /**combine bits together**/
    data->gx = (float)((int16_t)(((int16_t) gyroRaw[4])<<8 | ((int16_t) gyroRaw[5])))*gyroResolution*M_PI/180-gyroXOffset;
    data->gy = -(float)((int16_t)(((int16_t) gyroRaw[2])<<8 | ((int16_t) gyroRaw[3])))*gyroResolution*M_PI/180-gyroYOffset;
    data->gz = 0; ///< z axis broken
    ///data->gz = (float)((int16_t)(((int16_t) gyroRaw[0])<<8 | ((int16_t) gyroRaw[1])))*gyroResolution*M_PI/180-gyroZOffset;

    static uint8_t magRaw[6]; ///< x, y, z: lsb, msb = 3*2=6 bytes

    if(HAL_GPIO_ReadPin(DRDY_MAG_GPIO_Port,DRDY_MAG_Pin))
    {
        if(!ReadBurst(MAG, MAG_DATA_X_LSB, magRaw, 6))
        {
            return false;
        }
    }

    /**combine bits together**/
    int16_t mxRaw = (((int16_t) magRaw[1])<<(8-MAG_DATA_X_LSB_DATAX_LSB_POS) | ((int16_t) magRaw[0])>>MAG_DATA_X_LSB_DATAX_LSB_POS);
    int16_t myRaw = (((int16_t) magRaw[3])<<(8-MAG_DATA_Y_LSB_DATAY_LSB_POS) | ((int16_t) magRaw[2])>>MAG_DATA_Y_LSB_DATAY_LSB_POS);
    int16_t mzRaw = (((int16_t) magRaw[5])<<(8-MAG_DATA_Z_LSB_DATAZ_LSB_POS) | ((int16_t) magRaw[4])>>MAG_DATA_Z_LSB_DATAZ_LSB_POS);
    data->mx = (float)((mxRaw&0xfff)-(mxRaw&0x1000));
    data->my = (float)((myRaw&0xfff)-(myRaw&0x1000));
    data->mz = (float)((mzRaw&0x3fff)-(mzRaw&0x4000));
    /**compensate for offsets and sensitivity**/
    data->mx = (data->mx*magResolution-magXOffset)*magXScale;
    data->my = (data->my*magResolution-magYOffset)*magYScale;
    data->mz = (data->mz*magResolution-magZOffset)*magZScale;

    return true;
}

bool BMX055CalibrateAccGyro()
{
    ///*** ACC ***///
    if(!SetAccRange(ACC_PMU_RANGE_2G))
    {
        return false;
    }

    if(!WriteAddress(ACC, ACC_OFC_CTRL, ACC_OFC_CTRL_OFFSET_RESET_BIT))
    {
        return false;
    }
    /** wait for the operation to be complete **/
    uint8_t offsetCtrl = 0x00;
    while((offsetCtrl & ACC_OFC_CTRL_CAL_READY_BIT) == 0x00)
    {
        if(!ReadAddress(ACC, ACC_OFC_CTRL, &offsetCtrl))
        {
            return false;
        }
    }

    if(!WriteAddress(ACC, ACC_OFC_CTRL, ACC_OFC_CTRL_FAST_COMPENSATION_X))
    {
        return false;
    }
    /** wait for the operation to be complete **/
     offsetCtrl = 0x00;
    while((offsetCtrl & ACC_OFC_CTRL_CAL_READY_BIT) == 0x00)
    {
        if(!ReadAddress(ACC, ACC_OFC_CTRL, &offsetCtrl))
        {
            return false;
        }
    }

    if(!WriteAddress(ACC, ACC_OFC_CTRL, ACC_OFC_CTRL_FAST_COMPENSATION_Y))
    {
        return false;
    }
    /** wait for the operation to be complete **/
    offsetCtrl = 0x00;
    while((offsetCtrl & ACC_OFC_CTRL_CAL_READY_BIT) == 0x00)
    {
        if(!ReadAddress(ACC, ACC_OFC_CTRL, &offsetCtrl))
        {
            return false;
        }
    }

    if(!WriteAddress(ACC, ACC_OFC_CTRL, ACC_OFC_CTRL_FAST_COMPENSATION_Z))
    {
        return false;
    }
    /** wait for the operation to be complete **/
    offsetCtrl = 0x00;
    while((offsetCtrl & ACC_OFC_CTRL_CAL_READY_BIT) == 0x00)
    {
        if(!ReadAddress(ACC, ACC_OFC_CTRL, &offsetCtrl))
        {
            return false;
        }
    }

    if(!SetAccRange(ACC_PMU_DEFAULT_RANGE))
    {
        return false;
    }

    ///*** GYRO ***///

    if(!SetGyroRange(GYRO_RANGE_125_DEG))
    {
        return false;
    }

    if(!WriteAddress(GYRO, GYRO_FOC, GYRO_FOC_FAST_OFFSET_EN_X_BIT |
                                     GYRO_FOC_FAST_OFFSET_EN_Y_BIT |
                                     GYRO_FOC_FAST_OFFSET_EN_Z_BIT |
                                     GYRO_FOC_FAST_OFFSET_EN_BIT |
                                     GYRO_FOC_FAST_OFFSET_WORDLENGTH_256_SAMP))
    {
        return false;
    }

    if(!SetGyroRange(GYRO_RANGE_DEFAULT))
    {
        return false;
    }

    return true;
}

void Bmx055SetAccOffsets(float x, float y, float z)
{
    accXOffset = x;
    accYOffset = y;
    accZOffset = z;
}

void Bmx055SetGyroOffsets(float x, float y, float z)
{
    gyroXOffset = x;
    gyroYOffset = y;
    gyroZOffset = z;
}

void Bmx055SetMagOffsets(float x, float y, float z)
{
    magXOffset = x;
    magYOffset = y;
    magZOffset = z;
}

void Bmx055SetMagSensitivity(float x, float y, float z)
{
    magXScale = x;
    magYScale = y;
    magZScale = z;
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static bool ReadAddress(imuModules_t module, uint8_t address, uint8_t* data)
{
    if(module>=MODULE_COUNT)
    {
        return false;
    }

    if(bmxParams[module].minAddress > address || bmxParams[module].maxAddress < address)
    {
        return false;
    }

    uint8_t message = 0x80 | address; ///< bit 7: 1->read, 0->write

    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,0);
    if(HAL_OK != HAL_SPI_Transmit(hspi, &message, sizeof(message), 1000))
    {
        return false;
    }
    if(HAL_OK != HAL_SPI_Receive(hspi,data, sizeof(uint8_t), 1000))
    {
        return false;
    }
    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,1);

    return true;
}

static bool WriteAddress(imuModules_t module, uint8_t address, uint8_t data)
{
    if(module>=MODULE_COUNT)
    {
        return false;
    }

    if(bmxParams[module].minAddress > address || bmxParams[module].maxAddress < address)
    {
        return false;
    }

    uint8_t message = 0x7F & address; ///< bit 7: 1->read, 0->write

    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,0);
    if(HAL_OK != HAL_SPI_Transmit(hspi, &message, sizeof(message), 1000))
    {
        return false;
    }
    if(HAL_OK != HAL_SPI_Transmit(hspi, &data, sizeof(data), 1000))
    {
        return false;
    }
    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,1);

    return true;
}

static bool ReadBurst(imuModules_t module, uint8_t address, uint8_t* data, uint8_t size)
{
    if(module>=MODULE_COUNT)
    {
        return false;
    }

    if(bmxParams[module].minAddress > address || bmxParams[module].maxAddress < address)
    {
        return false;
    }

    uint8_t message = 0x80 | address; ///< bit 7: 1->read, 0->write

    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,0);
    if(HAL_OK != HAL_SPI_Transmit(hspi, &message, sizeof(message), 1000))
    {
        return false;
    }
    for(uint8_t i=0; i<size; i++)
    {
        if(HAL_OK != HAL_SPI_Receive(hspi,data+i, sizeof(uint8_t), 1000))
        {
            return false;
        }
    }
    HAL_GPIO_WritePin(bmxParams[module].csPort,bmxParams[module].csPin,1);

    return true;
}

static bool SetAccRange(uint8_t range)
{
    switch(range)
    {
    case ACC_PMU_RANGE_2G:
        accResolution = ACC_PMU_RESOLUTION_2G;
        break;
    case ACC_PMU_RANGE_4G:
        accResolution = ACC_PMU_RESOLUTION_4G;
        break;
    case ACC_PMU_RANGE_8G:
        accResolution = ACC_PMU_RESOLUTION_8G;
        break;
    case ACC_PMU_RANGE_16G:
        accResolution = ACC_PMU_RESOLUTION_16G;
        break;
    default:
        return false;
    }

    return WriteAddress(ACC, ACC_PMU_RANGE, range);

}

static bool SetGyroRange(uint8_t range)
{
    switch(range)
    {
    case GYRO_RANGE_125_DEG:
        gyroResolution = GYRO_RESOLUTION_125_DEG;
        break;
    case GYRO_RANGE_250_DEG:
        gyroResolution = GYRO_RESOLUTION_250_DEG;
        break;
    case GYRO_RANGE_500_DEG:
        gyroResolution = GYRO_RESOLUTION_500_DEG;
        break;
    case GYRO_RANGE_1000_DEG:
        gyroResolution = GYRO_RESOLUTION_1000_DEG;
        break;
    case GYRO_RANGE_2000_DEG:
        gyroResolution = GYRO_RESOLUTION_2000_DEG;
        break;
    default:
        return false;
    }

    return WriteAddress(GYRO, GYRO_RANGE, range);

}

static bool CheckConnection()
{
    uint8_t chipID = 0x00;
    if(!ReadAddress(ACC, ACC_BGW_CHIPID, &chipID))
    {
        return false;
    }

    if(chipID != ACC_ID_VALUE)
    {
        return false;
    }

    chipID = 0x00;
    if(!ReadAddress(GYRO, GYRO_CHIP_ID, &chipID))
    {
        return false;
    }

    if(chipID != GYRO_ID)
    {
        return false;
    }

    if(!WriteAddress(MAG, MAG_POWER_CONTROL, MAG_POWER_CONTROL_POWER_CONTROL_BIT))
    {
        return false;
    }

    HAL_Delay(10);  ///< wait for the magnetometer to power up

    chipID = 0x00;
    if(!ReadAddress(MAG, MAG_CHIP_ID, &chipID))
    {
        return false;
    }

    if(chipID != MAG_ID)
    {
        return false;
    }

    return true;
}


static bool NvmInit()
{
    float data = 0;
    if(EepromRead(EEPROM_ACC_OFFSET_X,  &data)) { accXOffset = data; }
    if(EepromRead(EEPROM_ACC_OFFSET_Y,  &data)) { accYOffset = data; }
    if(EepromRead(EEPROM_ACC_OFFSET_Z,  &data)) { accZOffset = data; }

    if(EepromRead(EEPROM_GYRO_OFFSET_X, &data)) { gyroXOffset = data; }
    if(EepromRead(EEPROM_GYRO_OFFSET_Y, &data)) { gyroYOffset = data; }
    if(EepromRead(EEPROM_GYRO_OFFSET_Z, &data)) { gyroZOffset = data; }

    if(EepromRead(EEPROM_MAG_OFFSET_X,  &data)) { magXOffset = data; }
    if(EepromRead(EEPROM_MAG_OFFSET_Y,  &data)) { magYOffset = data; }
    if(EepromRead(EEPROM_MAG_OFFSET_Z,  &data)) { magZOffset = data; }

    if(!MemoryRegisterVariable(EEPROM_ACC_OFFSET_X,  &accXOffset))  {return false;}
    if(!MemoryRegisterVariable(EEPROM_ACC_OFFSET_Y,  &accYOffset))  {return false;}
    if(!MemoryRegisterVariable(EEPROM_ACC_OFFSET_Z,  &accZOffset))  {return false;}

    if(!MemoryRegisterVariable(EEPROM_GYRO_OFFSET_X, &gyroXOffset)) {return false;}
    if(!MemoryRegisterVariable(EEPROM_GYRO_OFFSET_Y, &gyroYOffset)) {return false;}
    if(!MemoryRegisterVariable(EEPROM_GYRO_OFFSET_Z, &gyroZOffset)) {return false;}

    if(!MemoryRegisterVariable(EEPROM_MAG_OFFSET_X,  &magXOffset))  {return false;}
    if(!MemoryRegisterVariable(EEPROM_MAG_OFFSET_Y,  &magYOffset))  {return false;}
    if(!MemoryRegisterVariable(EEPROM_MAG_OFFSET_Z,  &magZOffset))  {return false;}

    return true;
}
