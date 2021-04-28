/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCC_ADC_Pin GPIO_PIN_1
#define VCC_ADC_GPIO_Port GPIOC
#define SPI_LPS_MISO_Pin GPIO_PIN_2
#define SPI_LPS_MISO_GPIO_Port GPIOC
#define SPI_LPS_MOSI_Pin GPIO_PIN_3
#define SPI_LPS_MOSI_GPIO_Port GPIOC
#define DEBUG_OUT_3_Pin GPIO_PIN_1
#define DEBUG_OUT_3_GPIO_Port GPIOA
#define DEBUG_OUT_2_Pin GPIO_PIN_2
#define DEBUG_OUT_2_GPIO_Port GPIOA
#define DEBUG_OUT_1_Pin GPIO_PIN_3
#define DEBUG_OUT_1_GPIO_Port GPIOA
#define LPS_CS_Pin GPIO_PIN_4
#define LPS_CS_GPIO_Port GPIOA
#define SPI_BMX_SCK_Pin GPIO_PIN_5
#define SPI_BMX_SCK_GPIO_Port GPIOA
#define SPI_BMX_MISO_Pin GPIO_PIN_6
#define SPI_BMX_MISO_GPIO_Port GPIOA
#define SPI_BMX_MOSI_Pin GPIO_PIN_7
#define SPI_BMX_MOSI_GPIO_Port GPIOA
#define CS_ACC_Pin GPIO_PIN_4
#define CS_ACC_GPIO_Port GPIOC
#define CS_GYRO_Pin GPIO_PIN_5
#define CS_GYRO_GPIO_Port GPIOC
#define CS_MAG_Pin GPIO_PIN_0
#define CS_MAG_GPIO_Port GPIOB
#define DRDY_MAG_Pin GPIO_PIN_1
#define DRDY_MAG_GPIO_Port GPIOB
#define SPI_LPS_SCK_Pin GPIO_PIN_10
#define SPI_LPS_SCK_GPIO_Port GPIOB
#define PWM_IN_1_Pin GPIO_PIN_12
#define PWM_IN_1_GPIO_Port GPIOB
#define PWM_IN_1_EXTI_IRQn EXTI15_10_IRQn
#define PWM_IN_2_Pin GPIO_PIN_13
#define PWM_IN_2_GPIO_Port GPIOB
#define PWM_IN_2_EXTI_IRQn EXTI15_10_IRQn
#define PWM_IN_3_Pin GPIO_PIN_14
#define PWM_IN_3_GPIO_Port GPIOB
#define PWM_IN_3_EXTI_IRQn EXTI15_10_IRQn
#define PWM_IN_4_Pin GPIO_PIN_15
#define PWM_IN_4_GPIO_Port GPIOB
#define PWM_IN_4_EXTI_IRQn EXTI15_10_IRQn
#define PWM_IN_5_Pin GPIO_PIN_6
#define PWM_IN_5_GPIO_Port GPIOC
#define PWM_IN_5_EXTI_IRQn EXTI9_5_IRQn
#define PWM_IN_6_Pin GPIO_PIN_7
#define PWM_IN_6_GPIO_Port GPIOC
#define PWM_IN_6_EXTI_IRQn EXTI9_5_IRQn
#define PWM_ESC_1_Pin GPIO_PIN_8
#define PWM_ESC_1_GPIO_Port GPIOA
#define PWM_ESC_2_Pin GPIO_PIN_9
#define PWM_ESC_2_GPIO_Port GPIOA
#define PWM_ESC_3_Pin GPIO_PIN_10
#define PWM_ESC_3_GPIO_Port GPIOA
#define PWM_ESC_4_Pin GPIO_PIN_11
#define PWM_ESC_4_GPIO_Port GPIOA
#define DEBUG_OUT_4_Pin GPIO_PIN_15
#define DEBUG_OUT_4_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
