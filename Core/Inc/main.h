/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern __IO uint32_t uwTick;
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
#define ENCODER_L1_Pin GPIO_PIN_0
#define ENCODER_L1_GPIO_Port GPIOA
#define ENCODER_L2_Pin GPIO_PIN_1
#define ENCODER_L2_GPIO_Port GPIOA
#define HC05_TX_Pin GPIO_PIN_2
#define HC05_TX_GPIO_Port GPIOA
#define HC05_RX_Pin GPIO_PIN_3
#define HC05_RX_GPIO_Port GPIOA
#define USB_TX_Pin GPIO_PIN_10
#define USB_TX_GPIO_Port GPIOB
#define USB_RX_Pin GPIO_PIN_11
#define USB_RX_GPIO_Port GPIOB
#define L1_Pin GPIO_PIN_12
#define L1_GPIO_Port GPIOB
#define L2_Pin GPIO_PIN_13
#define L2_GPIO_Port GPIOB
#define R1_Pin GPIO_PIN_14
#define R1_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_15
#define R2_GPIO_Port GPIOB
#define Motor_R_Pin GPIO_PIN_8
#define Motor_R_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_9
#define TRIG_GPIO_Port GPIOA
#define ECHO_Pin GPIO_PIN_10
#define ECHO_GPIO_Port GPIOA
#define ECHO_EXTI_IRQn EXTI15_10_IRQn
#define Motor_L_Pin GPIO_PIN_11
#define Motor_L_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define KEY2_Pin GPIO_PIN_15
#define KEY2_GPIO_Port GPIOA
#define KEY2_EXTI_IRQn EXTI15_10_IRQn
#define MPU_SCL_Pin GPIO_PIN_3
#define MPU_SCL_GPIO_Port GPIOB
#define MPU_SDA_Pin GPIO_PIN_4
#define MPU_SDA_GPIO_Port GPIOB
#define MPU_INT_Pin GPIO_PIN_5
#define MPU_INT_GPIO_Port GPIOB
#define MPU_INT_EXTI_IRQn EXTI9_5_IRQn
#define ENCODER_R1_Pin GPIO_PIN_6
#define ENCODER_R1_GPIO_Port GPIOB
#define ENCODER_R2_Pin GPIO_PIN_7
#define ENCODER_R2_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
