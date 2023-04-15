/**
  ******************************************************************************
  * @file		keypad4x4-scanning.h
  ******************************************************************************
  */

#ifndef __KEYPAD4X4_H
#define __KEYPAD4X4_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
#include "stm32f1xx_hal.h"//stm32f1xx_hal.h
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
//#include "delay.h"
//#include "Queue.h"

/*uint8_t key;
// USER CODE END Includes
uint8_t QueueCount = 0;
 Queue DMR_Queue;
unsigned char queue_msg1[2]={'\0'},event='\0';
unsigned char queue_msg[2]={'\0'};*/

// GPIO pin definitions for keypad columns (must on the same GPIO)
//#define KEYPAD_RCC_GPIO_COL		RCC_APB2Periph_GPIOA
#define KEYPAD_GPIO_COL				GPIOE
#define KEYPAD_PIN_COL1				GPIO_PIN_12
#define KEYPAD_PIN_COL2				GPIO_PIN_13
#define KEYPAD_PIN_COL3				GPIO_PIN_14
#define KEYPAD_PIN_COL4				GPIO_PIN_15
// GPIO pin definitions for keypad rows (must on the same GPIO)
//#define KEYPAD_RCC_GPIO_ROW		RCC_APB2Periph_GPIOA
#define KEYPAD_GPIO_ROW				GPIOE
#define KEYPAD_PIN_ROW1				GPIO_PIN_7
#define KEYPAD_PIN_ROW2				GPIO_PIN_8
#define KEYPAD_PIN_ROW3				GPIO_PIN_9
#define KEYPAD_PIN_ROW4				GPIO_PIN_10
#define KEYPAD_PIN_ROW5				GPIO_PIN_11
// Return value for no key pressed
#define KEYPAD_NO_PRESSED			NULL

/** Public function prototypes ---------------------------------------------- */
void unify_KeypadInit(void);
uint8_t unify_KeypadGetKey(void);


#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
