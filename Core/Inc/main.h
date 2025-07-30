/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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

/* USER CODE BEGIN EFP */
void OnSecondTick(void);
void OnTacho(uint32_t channel);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TACH01_Pin GPIO_PIN_4
#define TACH01_GPIO_Port GPIOC
#define TACH01_EXTI_IRQn EXTI4_IRQn
#define TACH00_Pin GPIO_PIN_0
#define TACH00_GPIO_Port GPIOB
#define TACH00_EXTI_IRQn EXTI0_IRQn
#define TACH07_Pin GPIO_PIN_13
#define TACH07_GPIO_Port GPIOB
#define TACH07_EXTI_IRQn EXTI15_10_IRQn
#define TACH03_Pin GPIO_PIN_9
#define TACH03_GPIO_Port GPIOA
#define TACH03_EXTI_IRQn EXTI9_5_IRQn
#define TACH06_Pin GPIO_PIN_12
#define TACH06_GPIO_Port GPIOA
#define TACH06_EXTI_IRQn EXTI15_10_IRQn
#define TACH04_Pin GPIO_PIN_10
#define TACH04_GPIO_Port GPIOC
#define TACH04_EXTI_IRQn EXTI15_10_IRQn
#define TACH05_Pin GPIO_PIN_11
#define TACH05_GPIO_Port GPIOC
#define TACH05_EXTI_IRQn EXTI15_10_IRQn
#define TACH02_Pin GPIO_PIN_5
#define TACH02_GPIO_Port GPIOB
#define TACH02_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
