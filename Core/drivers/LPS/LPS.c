/*****************************************************************************
 * @file /CalmarFlightController/Core/drivers/LPS/LPS.c
 *
 * @brief Source code
 * 
 * @author Michal Frankiewicz
 * @date Jun 22, 2021
 ****************************************************************************/

#include "drivers/LPS/LPS.h"

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/


/** LPS REGISTERS **/
#define REF_P_XL            (0x08U)     ///< reference pressure LSB
#define REF_P_L             (0x09U)     ///< reference pressure middle bits
#define REF_P_H             (0x0AU)     ///< reference pressure MSB


#define WHO_AM_I            (0x0FU)     ///< who am i register with value 0xbd
#define WHO_AM_I_VALUE      (0xBDU)     ///< who am i default value


#define RES_CONF            (0x10U)     ///< pressure and temperature resolution
typedef union{
    struct{
        uint8_t AVGP     : 2;       ///< pressure average config
        uint8_t AVGT     : 2;       ///< temp average config
        uint8_t RESERVED : 4;
    };
    uint8_t raw;
}RES_CONF_t;
#define RES_CONF_AVGP__8    (0x0U)
#define RES_CONF_AVGP__32   (0x1U)
#define RES_CONF_AVGP__128  (0x2U)
#define RES_CONF_AVGP__512  (0x3U)

#define RES_CONF_AVGT__8    (0x0U)
#define RES_CONF_AVGT__16   (0x1U)
#define RES_CONF_AVGT__32   (0x2U)
#define RES_CONF_AVGT__64   (0x3U)


#define CTRL_REG1           (0x20U)     ///< control register 1
typedef union{
    struct{
        uint8_t SIM      : 1;       ///< SPI Serial Interface Mode selection.Default value: 0(0: 4-wire interface; 1: 3-wire interface)
        uint8_t RESET_AZ : 1;       ///< Reset Autozero function. Default value: 0.(0: normal mode; 1: reset Autozero function)
        uint8_t DBU      : 1;       ///< block data update 0: continuous update;  1: output registers not updated until MSB and LSB have been read
        uint8_t DIFF_EN  : 1;       ///< interrupt generation enable, 0: disabled 1: enabled
        uint8_t ODR      : 3;       ///< output data rate
        uint8_t PD       : 1;       ///< power down ctrl 0: power down, 1: active mode

    };
    uint8_t raw;
}CTRL_REG1_t;
enum{
    CTRL_REG1_ODR__ONE_SHOT = 0,
    CTRL_REG1_ODR__1Hz,
    CTRL_REG1_ODR__7Hz,
    CTRL_REG1_ODR__12_5Hz,
    CTRL_REG1_ODR__25Hz
};

#define CTRL_REG2           (0x21U)     ///< control register 2
typedef union{
    struct{
        uint8_t ONE_SHOT    : 1;       ///< One shot mode enable. Default value: 0.(0: idle mode; 1: a new dataset is acquired)
        uint8_t AUTO_ZERO   : 1;       ///< Autozero enable. Default value: 0.(0: normal mode; 1: Autozero enabled)
        uint8_t SW_RESET    : 1;       ///< Software reset. Default value: 0.(0: normal mode; 1: software reset).The bit is self-cleared when the reset is completed
        uint8_t I2C_EN      : 1;       ///< I2C interface enabled. Default value 0.(0: I2C enabled;1: I2C disabled)
        uint8_t FIFO_MEAN   : 1;       ///< Register address automatically incremented during a multiple byte access with a serial interface (I2C or SPI). Default value 1.(0: disable; 1 enable)
        uint8_t STOP_ON_FTH : 1;       ///< Stop on FIFO threshold. Enable FIFO watermark level use. Default value 0(0: disable; 1: enable)
        uint8_t FIFO_EN     : 1;       ///< FIFO enable. Default value: 0.(0: disable; 1: enable)
        uint8_t BOOT        : 1;       ///< Reboot memory content. Default value: 0.(0: normal mode; 1: reboot memory content). The bit is self-cleared when the BOOT is completed.
    };
    uint8_t raw;
}CTRL_REG2_t;


#define CTRL_REG3           (0x22U)     ///< control register 3
typedef union{
    struct{
        uint8_t INT_S    : 2;       ///< Data signal on INT_DRDY pin control bits. Default value: 00
        uint8_t reserved : 4;       ///< reserved space do not change
        uint8_t PP_OD    : 1;       ///< Push-pull/open drain selection on interrupt pads. Default value: 0.(0: push-pull; 1: open drain)
        uint8_t INT_H_L  : 1;       ///< Interrupt active high, low. Default value: 0.(0: active high; 1: active low)
    };
    uint8_t raw;
}CTRL_REG3_t;
enum{
    CTRL_REG3_INT_S__DATA_SIGNAL = 0,
    CTRL_REG3_INT_S__P_HIGH,
    CTRL_REG3_INT_S__P_LOW,
    CTRL_REG3_INT_S__P_HIGH_LOW,
};


#define CTRL_REG4           (0x23U)     ///< control register 4
typedef union{
    struct{
        uint8_t DRDY     : 1;       ///< Data-ready signal on INT_DRDY pin. Default value: 0.(0: disable; 1: enable)
        uint8_t F_OVR    : 1;       ///< FIFO overrun interrupt on INT_DRDY pin to indicate that FIFO is full in FIFO mode or that an overrun occurred in Stream mode. Default value: 0.(0: disable; 1: enable)
        uint8_t F_FTH    : 1;       ///< FIFO threshold (watermark) status on INT_DRDY pin to indicate that FIFO is filled up to the threshold level. Default value: 0.(0: disable; 1: enable)
        uint8_t F_EMPTY  : 1;       ///< FIFO empty flag on INT_DRDY pin. Default value: 0.(0: disable; 1: enable)
        uint8_t reserved : 4;       ///< reserved space do not change
    };
    uint8_t raw;
}CTRL_REG4_t;


#define INTERRUPT_CFG       (0x24U)     ///< interrupt config
typedef union{
    struct{
        uint8_t PH_E     : 1;       ///< Enable interrupt generation on differential pressure high event. Default value: 0.(0: disable interrupt request; 1: enable interrupt request on measured differential pressure value higher than preset threshold)
        uint8_t PL_E     : 1;       ///< Enable interrupt generation on differential pressure low event. Default value: 0. (0: disable interrupt request; 1: enable interrupt request on measured differential pressure value lower than preset threshold)
        uint8_t LIR      : 1;       ///< Latch interrupt request to the INT_SOURCE (25h) register. Default value: 0. (0: interrupt request not latched; 1: interrupt request latched)
        uint8_t reserved : 5;       ///< reserved space do not change
    };
    uint8_t raw;
}INTERRUPT_CFG_t;

#define INT_SOURCE          (0x25U)     ///< interrupt source
typedef union{
    struct{
        uint8_t PH       : 1;       ///< Differential pressure High.(0: no interrupt has been generated; 1: High differential pressure event has occurred).
        uint8_t PL       : 1;       ///< Differential pressure Low.(0: no interrupt has been generated; 1: Low differential pressure event has occurred)
        uint8_t IA       : 1;       ///< Interrupt active.(0: no interrupt has been generated; 1: one or more interrupt events have been generated)
        uint8_t reserved : 5;       ///< reserved space do not change
    };
    uint8_t raw;
}INT_SOURCE_t;

#define STATUS_REG          (0x27U)     ///< status register
typedef union{
    struct{
        uint8_t T_DA      : 1;       ///< Temperature data available.(0: new data for temperature is not yet available; 1: new data for temperature is available)
        uint8_t P_DA      : 1;       ///< Pressure data available.(0: new data for pressure is not yet available; 1: new data for pressure is available)
        uint8_t reserved1 : 2;       ///< reserved space do not change
        uint8_t T_OR      : 1;       ///< Temperature data overrun.(0: no overrun has occurred; 1: a new data for temperature has overwritten the previous one)
        uint8_t P_OR      : 1;       ///< Pressure data overrun.(0: no overrun has occurred; 1: new data for pressure has overwritten the previous one)
        uint8_t reserved2 : 2;       ///< reserved space do not change
    };
    uint8_t raw;
}STATUS_REG_t;


#define PRESS_OUT_XL        (0x28U)     ///< pressure output value LSB
#define PRESS_OUT_L         (0x29U)     ///< pressure output value middle bits
#define PRESS_OUT_H         (0x2AU)     ///< pressure output value MSB

#define TEMP_OUT_L          (0x2BU)     ///< temperature out LSB
#define TEMP_OUT_H          (0x2CU)     ///< temperature out MSB

#define FIFO_CTRL           (0x2EU)     ///< fifo control
typedef union{
    struct{
        uint8_t WTM_POINT : 5;       ///< FIFO threshold (watermark) level selection
        uint8_t F_MODE    : 3;       ///< FIFO mode selection. Default value: 000
    };
    uint8_t raw;
}FIFO_CTRL_t;
enum{
    FIFO_CTRL_WTM_POINT_2  = 0x01,
    FIFO_CTRL_WTM_POINT_4  = 0x03,
    FIFO_CTRL_WTM_POINT_8  = 0x07,
    FIFO_CTRL_WTM_POINT_16 = 0x0F,
    FIFO_CTRL_WTM_POINT_32 = 0x1F,
};

enum{
    FIFO_CTRL_F_MODE__BYPASS = 0,
    FIFO_CTRL_F_MODE__FIFO,
    FIFO_CTRL_F_MODE_STREAM,
    FIFO_CTRL_F_MODE_STREAM_TO_FIFO,
    FIFO_CTRL_F_MODE_BYPASS_TO_STREAM,
    FIFO_CTRL_F_MODE_FIFO_MEAN = 0x6,
    FIFO_CTRL_F_MODE__BYPASS_TO_FIFO
};


#define FIFO_STATUS         (0x2FU)     ///< fifo status
typedef union{
    struct{
        uint8_t FSS        : 5;       ///< FIFO stored data level. (00000: FIFO empty; 10000: FIFO is full and has 32 unread samples)
        uint8_t EMPTY_FIFO : 1;       ///< Empty FIFO bit status. (0: FIFO not empty; 1: FIFO is empty)
        uint8_t OVR        : 1;       ///< Overrun bit status. (0: FIFO not full;1: FIFO is full and at least one sample in the FIFO has been overwritten)
        uint8_t FTH_FIFO   : 1;       ///< FIFO threshold status.(0: FIFO filling is lower than FTH level; 1: FIFO filling is equal or higher than FTH level)
    };
    uint8_t raw;
}FIFO_STATUS_t;


#define THS_P_L             (0x30U)     ///< LSB for trh value of pressure interrupt generation
#define THS_P_H             (0x31U)     ///< MSB for trh value of pressure interrupt generation

#define RPDS_L              (0x39U)     ///< pressure offset LSB
#define RPDS_H              (0x3AU)     ///< pressure offset MSB




/** OTHER DEFINES **/
#define PRESSURE_RESOLUTION (4096U) ///< LSB/hPa


/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static SPI_HandleTypeDef *hspi;

static float prevPressure = 0;

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief reads data from given register address
 *
 * @param [in] address
 * @param [out] data
 * @return true if successful
 */
static bool ReadAddress(uint8_t address, uint8_t* data);

/**@brief writes data to given address
 *
 * @param [in] address
 * @param [in] data
 * @return true if successful
 */
static bool WriteAddress(uint8_t address, uint8_t data);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool LPSInit(SPI_HandleTypeDef *HSPI)
{
    if(HSPI == NULL)
    {
        return false;
    }
    hspi = HSPI;

    HAL_GPIO_WritePin(LPS_CS_GPIO_Port,LPS_CS_Pin,1);

    if(!WriteAddress(CTRL_REG1 , ((CTRL_REG1_t){.PD=1}).raw)){return false;}

    uint8_t data=0;
    if(!ReadAddress(WHO_AM_I, &data)){return false;}
    if(data != WHO_AM_I_VALUE){return false;}

    if(!WriteAddress(CTRL_REG1 , ((CTRL_REG1_t){.PD = 1,
                                                .ODR = CTRL_REG1_ODR__25Hz,
                                                .DBU = 1}).raw))
    {
        return false;
    }

    if(!WriteAddress(RES_CONF,((RES_CONF_t){.AVGP = RES_CONF_AVGP__512,
                                            .AVGT = RES_CONF_AVGT__64}).raw))
    {
        return false;
    }

    if(!WriteAddress(CTRL_REG2,0x00)){return false;}

    if(!WriteAddress(REF_P_XL,0x00)){return false;}
    if(!WriteAddress(REF_P_L,0x00)){return false;}
    if(!WriteAddress(REF_P_H,0x00)){return false;}

    return true;
}

float LPSGetPressure()
{
    STATUS_REG_t status = {.raw = 0x00};
    ReadAddress(STATUS_REG, &(status.raw));
    if(status.P_DA == 0)
    {
        return prevPressure;
    }

    int32_t pressureRaw = 0;
    uint8_t data[3] = {0,0,0};

    ReadAddress(PRESS_OUT_XL, &data[0]);
    ReadAddress(PRESS_OUT_L, &data[1]);
    ReadAddress(PRESS_OUT_H, &data[2]);

    pressureRaw = ((int32_t)data[0]) | ((int32_t)data[1])<<8 | ((int32_t)data[2])<<16;

    pressureRaw = (pressureRaw&0x7FFFFF)-(pressureRaw&0x800000);

    return ((float)pressureRaw)/((float)PRESSURE_RESOLUTION);
}

/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

static bool ReadAddress(uint8_t address, uint8_t* data)
{
    ///TODO: check for address correctness
    uint8_t message = 0x80 | address; ///< bit 7: 1->read, 0->write

    HAL_GPIO_WritePin(LPS_CS_GPIO_Port,LPS_CS_Pin,0);
    if(HAL_OK != HAL_SPI_Transmit(hspi, &message, sizeof(message), 1000))
    {
        return false;
    }
    if(HAL_OK != HAL_SPI_Receive(hspi,data, sizeof(uint8_t), 1000))
    {
        return false;
    }
    HAL_GPIO_WritePin(LPS_CS_GPIO_Port,LPS_CS_Pin,1);

    return true;
}

static bool WriteAddress(uint8_t address, uint8_t data)
{
    ///TODO: check for address correctness
    uint8_t message = 0x7F & address; ///< bit 7: 1->read, 0->write

    HAL_GPIO_WritePin(LPS_CS_GPIO_Port,LPS_CS_Pin,0);
    if(HAL_OK != HAL_SPI_Transmit(hspi, &message, sizeof(message), 1000))
    {
        return false;
    }
    if(HAL_OK != HAL_SPI_Transmit(hspi, &data, sizeof(data), 1000))
    {
        return false;
    }
    HAL_GPIO_WritePin(LPS_CS_GPIO_Port,LPS_CS_Pin,1);

    return true;
}
