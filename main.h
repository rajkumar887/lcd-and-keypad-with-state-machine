/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stdbool.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define DEVICE_ADDRESS 	0x3C

// EEPROM ADDRESS (8bits)
#define EEPROM_ADDR 0xA0

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

// bool mode = true;

uint32_t ReadCounter(void);
void ClearCounter(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN0_Pin GPIO_PIN_3
#define EN0_GPIO_Port GPIOE
#define EN1_Pin GPIO_PIN_4
#define EN1_GPIO_Port GPIOE
#define EN2_Pin GPIO_PIN_5
#define EN2_GPIO_Port GPIOE
#define EN3_Pin GPIO_PIN_6
#define EN3_GPIO_Port GPIOE
#define PTT_IN_Pin GPIO_PIN_0
#define PTT_IN_GPIO_Port GPIOC
#define PTT_IN_EXTI_IRQn EXTI0_IRQn
#define SQUELCH_IN_Pin GPIO_PIN_1
#define SQUELCH_IN_GPIO_Port GPIOC
#define SQUELCH_IN_EXTI_IRQn EXTI1_IRQn
#define EMERGENCY_BUTTON_Pin GPIO_PIN_2
#define EMERGENCY_BUTTON_GPIO_Port GPIOC
#define Display_ON_OFF_Pin GPIO_PIN_3
#define Display_ON_OFF_GPIO_Port GPIOC
#define AMBE_RESET_Pin GPIO_PIN_1
#define AMBE_RESET_GPIO_Port GPIOA
#define T_R_CTRL_Pin GPIO_PIN_3
#define T_R_CTRL_GPIO_Port GPIOA
#define AUDIO_PA_SHDN_Pin GPIO_PIN_5
#define AUDIO_PA_SHDN_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_0
#define OLED_DC_GPIO_Port GPIOB
#define OLED_CS_Pin GPIO_PIN_1
#define OLED_CS_GPIO_Port GPIOB
#define ROW1_Pin GPIO_PIN_7
#define ROW1_GPIO_Port GPIOE
#define ROW2_Pin GPIO_PIN_8
#define ROW2_GPIO_Port GPIOE
#define ROW3_Pin GPIO_PIN_9
#define ROW3_GPIO_Port GPIOE
#define ROW4_Pin GPIO_PIN_10
#define ROW4_GPIO_Port GPIOE
#define ROW5_Pin GPIO_PIN_11
#define ROW5_GPIO_Port GPIOE
#define COL1_Pin GPIO_PIN_12
#define COL1_GPIO_Port GPIOE
#define COL2_Pin GPIO_PIN_13
#define COL2_GPIO_Port GPIOE
#define COL3_Pin GPIO_PIN_14
#define COL3_GPIO_Port GPIOE
#define COL4_Pin GPIO_PIN_15
#define COL4_GPIO_Port GPIOE
#define SYN_CS_Pin GPIO_PIN_12
#define SYN_CS_GPIO_Port GPIOB
#define SYN_SCLK_Pin GPIO_PIN_13
#define SYN_SCLK_GPIO_Port GPIOB
#define SYN_DATA_Pin GPIO_PIN_14
#define SYN_DATA_GPIO_Port GPIOB
#define RESET_Pin GPIO_PIN_10
#define RESET_GPIO_Port GPIOD
#define DSA_D1_Pin GPIO_PIN_11
#define DSA_D1_GPIO_Port GPIOD
#define DSA_D2_Pin GPIO_PIN_12
#define DSA_D2_GPIO_Port GPIOD
#define DSA_D3_Pin GPIO_PIN_13
#define DSA_D3_GPIO_Port GPIOD
#define DSA_D4_Pin GPIO_PIN_14
#define DSA_D4_GPIO_Port GPIOD
#define IRQN_Pin GPIO_PIN_15
#define IRQN_GPIO_Port GPIOD
#define IRQN_EXTI_IRQn EXTI15_10_IRQn
#define HL_POWER_Pin GPIO_PIN_8
#define HL_POWER_GPIO_Port GPIOA
#define HL_POWER_EXTI_IRQn EXTI9_5_IRQn
#define CS_Pin GPIO_PIN_15
#define CS_GPIO_Port GPIOA
#define INA_ALERT_Pin GPIO_PIN_3
#define INA_ALERT_GPIO_Port GPIOD
#define SYN_CE_Pin GPIO_PIN_4
#define SYN_CE_GPIO_Port GPIOD
#define SCLK_Pin GPIO_PIN_3
#define SCLK_GPIO_Port GPIOB
#define RDATA_Pin GPIO_PIN_4
#define RDATA_GPIO_Port GPIOB
#define CDATA_Pin GPIO_PIN_5
#define CDATA_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_6
#define OLED_RST_GPIO_Port GPIOB
#define LOCK_DETECT_Pin GPIO_PIN_7
#define LOCK_DETECT_GPIO_Port GPIOB
#define G_LED_Pin GPIO_PIN_0
#define G_LED_GPIO_Port GPIOE
#define R_LED_Pin GPIO_PIN_1
#define R_LED_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
