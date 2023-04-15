/*
 * INA260_Battery.c
 *
 *  Created on: 03-Nov-2022
 *      Author: kalyan
 */

#include "Battery.h"
#include "Analogue.h"
#include "cmx7341.h"
#include "spi_bitbang.h"
#include "u8g2.h"
#include "lcd.h"
#include "stdio.h"
#include "i2c.h"

uint8_t Buffp[2];
uint8_t buf[2];
extern u8g2_t u8g2;
void Unify_Battery_Init()
{


	I2C_Read(Manuf_ID_Reg_Add,(uint8_t*)Buffp);                    // Contains unique manufacturer identification number.
	//I2C_Read(Config_Reg_Add,&Buffp);                     /* All-register reset, shunt voltage and bus voltage ADC conversion times and
                                                            //  averaging, operating mode. */

    // I2C_Read(Bus_Vol_Reg_Add,&Buffp);                    // Bus voltage measurement data.
	//I2C_Read(Curr_Reg_Add,&Buffp);                       // Contains the value of the current flowing through the shunt resistor.
	//I2C_Read(Power_Reg_Add,&Buffp);                     //  Contains the value of the calculated power being delivered to the load.
	//I2C_Read(Alert_Limit_Reg_Add,&Buffp);               /* Contains the limit value to compare to the selected Alert function.*/
	Unify_Battery_Moniter();
	Unify_Current_Alert_Message();
}

/* I2C Read Function */
uint8_t *I2C_Read(uint8_t ADDR,uint8_t *i2cBif)
{
	uint8_t i2cBuf[2];
	i2cBuf[0] = ADDR;
	HAL_I2C_Master_Transmit(&hi2c2, Unify_ADD, i2cBuf, 1, 100);
	HAL_I2C_Master_Receive(&hi2c2, Unify_ADD, i2cBif, 2, 100);
//	HAL_UART_Transmit(&huart1,i2cBif,2, 100 );
	return i2cBif;

}

/* I2C Write Function */
void I2C_Write(uint8_t ADDR, uint8_t data)
{
	uint8_t i2cData[2];
	i2cData[0] = ADDR;
	i2cData[1] = data;
	HAL_I2C_Master_Transmit(&hi2c2, Unify_ADD, i2cData, 2,100);

}



void Unify_Battery_Moniter()
{

	int32_t voltage = 0;

	//double cal_val=(double)voltage;
	char str[10];
	//uint16_t ret;
	//uint8_t buffer[100];
	int32_t hexVal;
	//int32_t hex;
	I2C_Read(Bus_Vol_Reg_Add,(uint8_t*)Buffp);
	//HAL_UART_Transmit(&huart1,ret,2, 100 );

	hexVal = (Buffp[0]<<8)|Buffp[1];
	//sprintf(buffer,"%x",ret);
	//hexVal = atoi(ret);
	voltage = hexVal * 1.2574;

	float cal_val = (float)voltage/1000;
			sprintf(str,"%.2fV",cal_val);
			//printf("voltage:%s\r\n",str);
			u8g2_DrawStr(&u8g2, 96, 10, str);

	if((voltage <= 8400) && (voltage > 7400))
	{
		/* LCD Screen  Level 5*/
		//battery_monitaring_levels(Battery_full_bits);
	/*	float cal_val = (float)voltage/1000;
		sprintf(str,"%.2fV",cal_val);
		printf("voltage:%s\r\n",str);
		u8g2_DrawStr(&u8g2, 96, 10, str);*/
	}

	/***voltage value 7***/
	else if((voltage <= 7400) && (voltage > 6400))
	{
		/* LCD Screen  Level 4*/
		//u8g2_NextPage(&u8g2);
		//battery_monitaring_levels(Battery_low3_bits);
		/*float cal_val = (float)voltage/1000;
		sprintf(str,"%.2fV",cal_val);
		printf("voltage:%s\r\n",str);
		u8g2_DrawStr(&u8g2, 96, 10, str);*/
	}

	else if((voltage <= 6400) && (voltage > 6000))
	{
		/* LCD Screen  Level 4*/
		//u8g2_NextPage(&u8g2);
		//battery_monitaring_levels( Battery_low2_bits);
		/*float cal_val = (float)voltage/1000;
		sprintf(str,"%.2fV",cal_val);
		printf("voltage:%s\r\n",str);
		u8g2_DrawStr(&u8g2, 96, 10, str);*/
	}
	/***voltage value 6.8***/
	else if(voltage <= 6000 && voltage > 5500)
	{
		/* LCD Screen  Level 3*/
		//u8g2_NextPage(&u8g2);
		//battery_monitaring_levels( Battery_low1_bits);
		/*float cal_val = (float)voltage/1000;
		sprintf(str,"%.2fV",cal_val);
		printf("voltage:%s\r\n",str);
		u8g2_DrawStr(&u8g2, 96, 10, str);
*/	}

	/***voltage value 6.6***/
	else if(voltage <=5500 && voltage >=5000)
	{
		/* LCD Screen  Level 2*/
		//u8g2_NextPage(&u8g2);
		//battery_monitaring_levels( Battery_empty_bits);
		/*float cal_val = (float)voltage/1000;
		sprintf(str,"%.2fV",cal_val);
		printf("voltage:%s\r\n",str);
		u8g2_DrawStr(&u8g2, 96, 10, str);*/
	}
//	u8g2_NextPage(&u8g2);
	/*else
	{
		u8g2_SetPowerSave(&u8g2, 1);
	}*/

}

void Unify_Current_Alert_Message()
{
	int32_t Current = 0;
	int32_t hexVal;
	uint16_t retVal ;
	uint8_t Buff[2];
	uint8_t Low;
	I2C_Read(Curr_Reg_Add,(uint8_t*)Buffp);
	hexVal = (Buffp[0]<<8)|Buffp[1];
	Current = hexVal * 1.25;
	//uint16_t pData = 0x4000;
	//HAL_I2C_Mem_Read(&hi2c2, Unify_ADD, Mask_Enable_Reg_Add, 1,(uint8_t *)&pData, 2, 1000);

	/*Low = HAL_GPIO_ReadPin(INA_ALERT_GPIO_Port,INA_ALERT_Pin);
	I2C_Read(Alert_Limit_Reg_Add,(uint8_t*)Buff);
	retVal = (Buff[0]<<8)|Buff[1];*/
	I2C_Read(Mask_Enable_Reg_Add,(uint8_t*)Buff);
	retVal = (Buff[0]<<8)|Buff[1];
	if(Current <= 600)
	{
		I2C_Read(Alert_Limit_Reg_Add,(uint8_t*)Buff);
		retVal = (Buff[0]<<8)|Buff[1];
		if(retVal == Low){
			//HAL_UART_Transmit(&huart1,(uint8_t *)"Alert Current\n\r",15, 100 );
			//current_monirting();
			//power_amplifer();
			printf("protect PA\r\n");
		}
	}
	else if(Current > 1800)
	{
		I2C_Read(Alert_Limit_Reg_Add,(uint8_t*)Buff);
		retVal = (Buff[0]<<8)|Buff[1];
		if(retVal == Low)
		{
			//Unify_Analogue_Rx();
			//power_amplifer();
			printf("protect PA\r\n");
			Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000);

			//HAL_UART_Transmit(&huart1,(uint8_t *)"Alert Current\n\r",15, 100 );
			//current_monirting();
		}
	}

}




