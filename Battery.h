/*
 * Unify_Battery.h
 *
 *  Created on: 03-Nov-2022
 *      Author: kalyan
 */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include <string.h>

//#include "u8g2/u8g2.h"
//#include"u8g2.h"
#include "lcd.h"
//#include "u8g2_stm32f1.h"
//#include "u8x8.h"
#include"displaybitmaps.h"
/* I2C Slave  Device Address */

#define Unify_ADD 0x88

/* Register Maps */

#define Config_Reg_Add				0x00
#define Curr_Reg_Add		    	0x01
#define Bus_Vol_Reg_Add				0x02
#define Power_Reg_Add				0x03
#define Mask_Enable_Reg_Add			0x06
#define Alert_Limit_Reg_Add			0x07
#define Manuf_ID_Reg_Add			0xFE
#define Die_ID_Reg_Add				0xFF


void Unify_Battery_Init();
uint8_t *I2C_Read(uint8_t ADDR,uint8_t *i2cBif);
void I2C_Write(uint8_t ADDR, uint8_t data);
void Unify_Battery_Moniter();
void Unify_Current_Alert_Message();
#endif /* INC_BATTERY_H_ */
