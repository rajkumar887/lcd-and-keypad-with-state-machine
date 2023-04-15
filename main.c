/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include <cmx7341.h>
#include <keypad5x4-scanning.h>
#include <spi_bitbang.h>
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include "stdbool.h"
#include "radio.h"
#include "DMR.h"
#include "function_image.h"
#include "ADF4351.h"
#include "rotary.h"
#include "Analogue.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint16_t squelch_flag, squelch_gpio_flag;
extern bool DMR_Mode;
extern uint16_t LcControlHeader[9];
extern bool ptt_mode; // mode

void readPTT(void)
{

	static uint8_t prev_ptt_pos;
	bool ptt_status;

	ptt_status = HAL_GPIO_ReadPin(PTT_IN_GPIO_Port,PTT_IN_Pin);

	if(ptt_status != prev_ptt_pos)
	{
		prev_ptt_pos = ptt_status;
		if(ptt_mode && (!ptt_status) )
		{
			ptt_mode = false;
			printf("T\n\r");
			if(DMR_Mode)
				Radio.RadioState = START_DMR_TX;
			else
				Radio.RadioState = START_ANALOGUE_TX;
		}
		if(ptt_status && !ptt_mode)
		{
			ptt_mode = true;
			printf("R\n\r");
			if(DMR_Mode)
				Radio.Terminate = 1;
			//Radio.RadioState = START_DMR_RX;
			else
			{
				Radio.Terminate = 1;
			}
		}
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	uint8_t prev_rot_pos,rotary_pos,prev_keypad_pos,keypad_pos,sel_call_pos;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_TIM13_Init();
	MX_USART3_UART_Init();
	MX_USART1_UART_Init();
	MX_I2C2_Init();
	/* USER CODE BEGIN 2 */

	unify_KeypadInit();

	/* Initialize Rotary switch parameters */
	unify_ChannelParameters();

	/* Apply DSA attenuation */
	HAL_GPIO_WritePin(DSA_D1_GPIO_Port,DSA_D1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DSA_D2_GPIO_Port,DSA_D2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DSA_D3_GPIO_Port,DSA_D3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(DSA_D4_GPIO_Port,DSA_D4_Pin,GPIO_PIN_SET);

	//G_LED OFF & R_LED OFF
	HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_RESET);

	/*Initialize/start timer interrupt service*/
	HAL_TIM_Base_Start_IT(&htim13);

	/* Initialize synthesizer and basic register configurations */
	ADF4351_Init();
	ADF4351_Reg_Config();
	unify_lcd_init();
	
	 unify_logo_screen();
	 unify_KeypadInit();
	 Unify_Current_Alert_Message();

  _idu_mmi_init();
  _idu_mmi_start();
  
  while(1)
  {

  }
	while(1)
	{
		/* Get rotary swicth position */
		rotary_pos = unify_RotaryGetKey();
		if((rotary_pos != 0x00) && (prev_rot_pos != rotary_pos))
		{
			/* Select the Configuration depending on rotary switch position */
			HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_SET);
			rotary_information(rotary_pos);
			HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
		}
		prev_rot_pos = rotary_pos;

		keypad_pos = unify_KeypadGetKey();
		if((keypad_pos != 0x00) && (prev_keypad_pos != keypad_pos))
		{
			printf("present key:%c\n\r",keypad_pos);
			if(keypad_pos >= 0x30 && keypad_pos <= 0x39)
			{
				sel_call_pos = keypad_pos;
			}
			if(keypad_pos == 0x2A)
			{
				sel_call_pos = keypad_pos;
			}
			if(keypad_pos == 0x45)
			{
				LcControlHeader[11] = (sel_call_pos - 0x30);
			}

		}
		prev_keypad_pos = keypad_pos;

		/* Squelch functionality in FM mode */
		if (squelch_flag && squelch_gpio_flag && !DMR_Mode)
		{
			Unify_Squelch();
			squelch_flag=0;
		}
		/* Main application for FM & DMR */
		DoRadio();
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
