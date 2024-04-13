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
#include "stm32u5xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define ACCEL_INT1_Pin GPIO_PIN_1
#define ACCEL_INT1_GPIO_Port GPIOH
#define SCREEN_RST_Pin GPIO_PIN_0
#define SCREEN_RST_GPIO_Port GPIOC
#define IrDA_TX_Pin GPIO_PIN_0
#define IrDA_TX_GPIO_Port GPIOA
#define IrDA_RX_Pin GPIO_PIN_1
#define IrDA_RX_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA
#define EEPROM_CSB_Pin GPIO_PIN_8
#define EEPROM_CSB_GPIO_Port GPIOA
#define FLASH_CSB_Pin GPIO_PIN_15
#define FLASH_CSB_GPIO_Port GPIOA
#define BUT3_Pin GPIO_PIN_10
#define BUT3_GPIO_Port GPIOC
#define BUT2_Pin GPIO_PIN_11
#define BUT2_GPIO_Port GPIOC
#define BUT1_Pin GPIO_PIN_12
#define BUT1_GPIO_Port GPIOC
#define ACCEL_CSB_Pin GPIO_PIN_5
#define ACCEL_CSB_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOH
#define BAT_LO_Pin GPIO_PIN_8
#define BAT_LO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
