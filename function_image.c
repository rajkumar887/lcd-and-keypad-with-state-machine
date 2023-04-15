#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "stdint.h"
#include "spi_bitbang.h"
#include "7241-7341-2.4.0.2.h"
#include "7341-1.1.7.1.h"
#include "main.h"
#include "cmx7341.h"
#include "radio.h"
#include "usart.h"
#include "rotary.h"
#include "Analogue.h"

uint16_t ptr =0, length = 0, data = 0, fi_status = 0, i = 0;
uint16_t cmx_id = 0, db1_hi = 0, db1_lo = 0, db2_hi = 0, db2_lo = 0, fi_version = 0;

bool mode = true, ptt_mode = true;
bool DMR_Mode;
bool squelch_mode =  true, HL_mode = true;
uint16_t squelch_flag, squelch_gpio_flag = 1,HL_flag = 1;

/*-------------------------------------------------------------------
/// TIMER_CallBack
--------------------------------------------------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{

	squelch_flag = 1;

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{/*
	bool ptt_status;

	if(GPIO_Pin == PTT_IN_Pin) // If The INT Source Is EXTI Line9 (D1 Pin)
	{
		ptt_status = HAL_GPIO_ReadPin(PTT_IN_GPIO_Port,PTT_IN_Pin);

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
	}*/

	/*if(GPIO_Pin == IRQN_Pin) // If The INT Source Is EXTI Line9 (D1 Pin)
	{
		printf("I");
		cmx7341.IRQn = 1;	//set flag for foreground
	}*/

	if(GPIO_Pin == SQUELCH_IN_Pin)
	{
		if(!DMR_Mode)
		{
			if(squelch_mode)
			{
				squelch_mode = false;
				printf("Audio PA enabled\n\r");
				Unify_AudioPAon();
				//key_SQUELCH = 29;
				squelch_gpio_flag = 0;
				//HAL_UART_Transmit(&huart1,(uint8_t*)"SQL_OFF \n\r",10,100);
			}
			else
			{
				squelch_mode = true;
				squelch_gpio_flag = 1;
				printf("Audio PA disabled\n\r");
				Unify_AudioPAoff();
				//key_SQUELCH = 30;
				//HAL_UART_Transmit(&huart1,(uint8_t*)"SQL_ON \n\r",9,100);
			}
		}
	}
	if(GPIO_Pin == HL_POWER_Pin)
	{
		if (HL_mode)
		{
			HL_mode = false;
			HL_flag = 0;
			Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000);
			Spi_Write_HalfWord(AUX_DAC1_DATA, 0x837F);//5w
			printf("Switched to 5W\n\r");
		}
		else
		{
			HL_mode = true;
			HL_flag = 1;
			Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000);
			Spi_Write_HalfWord(AUX_DAC1_DATA, 0x8240); //1w
			printf("Switched to 1W\n\r");

		}
	}
}

void LoadFI(uint8_t mode)
{
	uint16_t db1_ptr,db1_length,db2_ptr,db2_length,activate_ptr,activate_length;
	printf("FI loading initiated\n\r");
	if(mode)
	{
		db1_ptr = DMR_DB1_PTR;
		db1_length = DMR_DB1_LEN;
		db2_ptr = DMR_DB2_PTR;
		db2_length = DMR_DB2_LEN;
		activate_ptr = DMR_ACTIVATE_PTR;
		activate_length = DMR_ACTIVATE_LEN;
		DMR_Mode = true;
		Radio.RadioState = START_DMR_RX;
	}
	else
	{
		db1_ptr = FM_DB1_PTR;
		db1_length = FM_DB1_LEN;
		db2_ptr = FM_DB2_PTR;
		db2_length = FM_DB2_LEN;
		activate_ptr = FM_ACTIVATE_PTR;
		activate_length = FM_ACTIVATE_LEN;
		DMR_Mode = false;
		squelch_mode = true;
		Radio.RadioState = START_ANALOGUE_RX;
	}
	/* Hard reset */
	printf("cmx7341 Hard reset is initiated\n\r");
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET);
	HAL_Delay(3);
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET);
	HAL_Delay(3);
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET);
	HAL_Delay(3);
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET);
	HAL_Delay(3);
	printf("cmx7341 Hard reset completed\n\r");
	/* Checking the C-BUS communication*/
	Spi_Write_HalfWord( POWERDOWN_CTRL, 0x0004);
	data = SPI_Read_HalfWord(POWERDOWN_READ);

	while(!(data == 0x0004))
		data = SPI_Read_HalfWord(POWERDOWN_READ);

	if(data == 0x0004)
		printf("CBUS communication is successful\n\r");
	else
		printf("CBUS communication error\n\r");
	data = 0;

	/* soft or general reset */
	printf("cmx7341 soft reset initiated\n\r");
	Spi_Write_NOData_ChipCMX7341(SOFT_RESET);
	printf("cmx7341 soft reset completed\n\r");
	/*Read or Flush the status register*/
	fi_status = 0;
	while(!(fi_status & PROGBIT))
		fi_status = SPI_Read_HalfWord(STATUS);


	/*Load DB1*/
	printf("loading DB1\n\r");
	Spi_Write_HalfWord(TXFIFO_WORD,db1_length);
	Spi_Write_HalfWord(TXFIFO_WORD,db1_ptr);
	while (!(db1_length == 0))
	{
		if(mode)
			Spi_Write_HalfWord(TXFIFO_WORD, dmr_db1[i]);
		else
			Spi_Write_HalfWord(TXFIFO_WORD, fm_db1[i]);

		i++;
		db1_length--;
		if(db1_length == 0)
		{
			db1_length = 0;
		}
	}
	i=0;

	/*Load DB2*/
	printf("loading DB2\n\r");
	Spi_Write_HalfWord(TXFIFO_WORD, db2_length);

	Spi_Write_HalfWord(TXFIFO_WORD, db2_ptr);
	while (!(db2_length == 0))
	{
		if(mode)
			Spi_Write_HalfWord(TXFIFO_WORD, dmr_db2[i]);
		else
			Spi_Write_HalfWord(TXFIFO_WORD, fm_db2[i]);
		i++;
		db2_length--;
		if(db2_length == 0)
		{
			db2_length = 0;
		}

	}
	i=0;

	/*Load activate ptr & Length*/
	printf("loading Activate ptr & length\n\r");
	Spi_Write_HalfWord(0xB7, activate_length);
	Spi_Write_HalfWord(0xB6, activate_ptr);
	HAL_Delay(10);

	Spi_Write_HalfWord(PROG_REG, 0x0001);
	HAL_Delay(30);
	data = SPI_Read_HalfWord(STATUS);
	/*Read or Flush status register*/
	fi_status = 0;
	while(!(fi_status & PROGBIT))
		fi_status = SPI_Read_HalfWord(STATUS);
	/*read cmx 7341 ID*/
	cmx_id = SPI_Read_HalfWord(DEVICE_TYPE);
	if(cmx_id == 0x7341)
		printf("Base band processor ID:%X\n\r",cmx_id);
	else
		printf("Base band processor detection failed\n\r");
	/*Read FI version*/
	fi_version = SPI_Read_HalfWord (FI_VERSION);
	if(fi_version == 0x1171)
		printf("FM FI version:%X\n\r",fi_version);
	else if(fi_version == 0x2402)
		printf("DMR FI version:%X\n\r",fi_version);
	else
		printf("FI version detection failed\n\r");

	/*verify db1, db2 check sums*/
	db2_hi = SPI_Read_HalfWord (XCHECK_HI);
	db2_lo = SPI_Read_HalfWord (XCHECK_LO);
	db1_hi = SPI_Read_HalfWord (PCHECK_HI);
	db1_lo = SPI_Read_HalfWord (PCHECK_LO);
	HAL_Delay(10);

	/* Loading the activation code */
	printf("loading activation codes\n\r");
	if(mode)
	{
		Spi_Write_HalfWord(PROG_REG, 0x50AA);
		Spi_Write_HalfWord(PROG_REG, 0xE5F6);
	}
	else
	{
		Spi_Write_HalfWord(PROG_REG, 0xAB65);
		Spi_Write_HalfWord(PROG_REG, 0x7BAF);
	}

	fi_status = 0;
	while(!(fi_status & PROGBIT))
		fi_status = SPI_Read_HalfWord(STATUS);

	printf("FI loading completed\n\r");

}
