/*
 * Analogue.c
 *
 *  Created on: 19-Aug-2022
 *      Author: dmr
 */
#include <cmx7341.h>
#include <spi_bitbang.h>
#include "stdint.h"
#include "Analogue.h"
#include "usart.h"
#include "tim.h"
#include "main.h"
#include <stdio.h>
#include "usart.h"
#include "rotary.h"

uint16_t Unify_check_status = 0 , Unify_dc_offset = 0;
uint16_t Unify_squelch = 0,Unify_sql = 0, Unify_frq = 0, Unify_rssi = 0,Unify_agc_rssi = 0;
uint16_t Unify_IChannel, Unify_QChannel;
uint32_t Unify_count;
uint16_t AGC_RSSI;
uint8_t agc,rssi,prev_agc,prev_rssi;

uint16_t check_FM_Status=0,prev_FM_Status=0;
char rssi1[12], agc1[10];

extern uint16_t count;
extern struct Channel_parameters Channel_pointer;


void Check_FM_Mode_Rx_Status(void)
{
	/*check_FM_Status = SPI_Read_HalfWord(CBUS_9B);
	if(check_FM_Status!=prev_FM_Status)
	{
		HAL_UART_Transmit(&huart1,&check_FM_Status,1,100);
		HAL_UART_Transmit(&huart1,"\n\r",2,100);
	}
	prev_FM_Status = check_FM_Status;*/

	AGC_RSSI = SPI_Read_HalfWord(AUXDATA1);
	rssi = (Unify_agc_rssi) & 0xFF;
	agc = (Unify_agc_rssi>>8) & 0xF;

	if(rssi!=prev_rssi)
	{
		/*HAL_UART_Transmit(&huart1,"RSSI: ",6,100);
		HAL_UART_Transmit(&huart1,&rssi,1,100);
		HAL_UART_Transmit(&huart1,"\n\r",2,100);*/
		sprintf(rssi1,"RSSI = %X\n\r",rssi);
		HAL_UART_Transmit(&huart1,(uint8_t*)rssi1,10,100);
	}

	if(agc!=prev_agc)
	{
		/*HAL_UART_Transmit(&huart1,"AGC: ",5,100);
		HAL_UART_Transmit(&huart1,&agc,1,100);
		HAL_UART_Transmit(&huart1,"\n\r",2,100);*/
		sprintf(agc1,"AGC = %X\n\r",agc);
		HAL_UART_Transmit(&huart1,(uint8_t*)agc1,9,100);
	}
	prev_rssi = rssi;
	prev_agc = agc;
}

void Unify_Start_Analogue(void)
{
	Spi_Write_HalfWord(POWERDOWN_CTRL, 0xBFC4); //MOD1 & MOD2

	Spi_Write_HalfWord(MODE_CONTROL,0x0000); //RESET

	Unify_config_cmx994();

	Spi_Write_HalfWord(ANALOG_LEVEL, 0x4066);

	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x0258);

	Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000);

	Spi_Write_HalfWord(AUX_DAC1_DATA, 0x8240); //1w

	while(!(Unify_check_status & PROGBIT))
		Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for Status

	Unify_Analogue_ProgBlock0();
	Unify_Analogue_ProgBlock2();
	Unify_Analogue_ProgBlock6();

	while(!(Unify_check_status & PROGBIT))
		Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for Status

	Unify_AgcConfig();  //AGC Configuration

	Spi_Write_HalfWord(ANALOG_OP_GAIN ,0x000F);  //MOD1 output polarity True, MOD1 Output Attenuation >40db, MOD2 output polarity True, MOD2 Output Attenuation >40db, Audio Output Attenuation 12.8db

	Spi_Write_HalfWord(ANALOG_CTRL,0x0055);

	printf("FM Mode\n\r");
	return;
}
void Unify_Analogue_Tx(void)
{
	Unify_AudioPAoff();
	Spi_Write_HalfWord(ANALOG_OP_GAIN ,0x6600);//0x6600---------->770F
	Spi_Write_HalfWord(ANALOG_CTRL,0x0055);
	Spi_Write_HalfWord(POWERDOWN_CTRL, 0xBFC4);
	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x02BC);

	/**DAC1**/
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000);
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x8000);
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x8240); //1w
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x833F);//2.5v
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x835F);//2.75v
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x837F);//2.85v
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x83CF);//3.1v
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x83BF);//3v
	//Spi_Write_HalfWord(AUX_DAC1_DATA, 0x83FF);//3.3v

	Spi_Write_HalfWord(ANALOG_LEVEL, 0x2100);

	//Spi_Write_HalfWord(ANALOG_LEVEL, 0x4066);



	if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
	{
		Spi_Write_HalfWord(ANALOG_LEVEL, 0x6200);
		HAL_Delay(10);
		Spi_Write_HalfWord(0xB6, 0x0080);
		Spi_Write_HalfWord(MODE_CONTROL, 0x0202); //Tx Mode Setup
		while(!(Unify_check_status & PROGBIT))
			Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for Status

		count = 0;
		while(count<5)
		{
			Spi_Write_HalfWord(0xB6, count);
			HAL_Delay(30);
			Spi_Write_HalfWord(0xB6, 0x0080);
			HAL_Delay(30);
			count++;
		}
	}
	printf("Call initiated\n\r");

	Spi_Write_HalfWord(MODE_CONTROL, 0x8002);

	printf("Tx Mode Start\n\r");

	return;
}

void Unify_Analogue_Rx(void)
{

	Unify_AudioPAoff();
	Spi_Write_HalfWord(AUX_DAC1_DATA, 0x0000); //Reset DAC1

	while(!(Unify_check_status & PROGBIT))
		Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for status


	Spi_Write_HalfWord(POWERDOWN_CTRL,0xBFC4);  //ALT amp enable, DISC amp enable, Input 1 enable, Output 1 enable ,Output 2 enable,MOD 1 enable ,MOD 2 enable,Audio Output enable , BIAS block enable,Input 2 enable

	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x0258); //Output 1 -> MOD1,VBIAS -> MOD2,Output 1 from SPI -> AUDIO,Input 1 Signal Routing ---> DISC,Input 2 Signal Routing --->ALT

	Spi_Write_HalfWord(MODEM_CONFIG,0xF000); //Enable Voice / Mic AGC , Enable Voice Compander, Enable Voice Pre-emphasis ,Enable Voice HPF

	Spi_Write_HalfWord(ANALOG_LEVEL, 0x3400);
	//Spi_Write_HalfWord(ANALOG_LEVEL, 0x5200);

	Spi_Write_HalfWord(AUX_CONFIG,0x3023);  //Selecting AUX DATA 1 for Reading AGC & RSSI in $A9 , AUX DATA 2 for Reading Squelch in $AA


	if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		Spi_Write_HalfWord(MODE_CONTROL,0x0201); //Enable Voice / audio processing & Rx Mode
	else
		Spi_Write_HalfWord(MODE_CONTROL,0x8001);

	Spi_Write_HalfWord(ANALOG_OP_GAIN ,0x000F);  //MOD1 output polarity True, MOD1 Output Attenuation >40db, MOD2 output polarity True, MOD2 Output Attenuation >40db, Audio Output Attenuation 12.8db

	printf("Rx Mode Start\n\r");
	return;
}

void Unify_Squelch(void)
{

	/*char Unify_squelch1[20];
	char Unify_rssi1[12], Unify_agc1[10];*/

	//typedef unsigned char BYTE;
	//BYTE AGC,RSSI;
	Unify_agc_rssi = SPI_Read_HalfWord(AUXDATA1);
	//RSSI = (Unify_agc_rssi) & 0xFF;
	//AGC = (Unify_agc_rssi>>8) & 0xF;

	/*sprintf(Unify_agc1,"AGC = %X\n\r",AGC);
	HAL_UART_Transmit(&huart1,(uint8_t*)Unify_agc1,9,100);

	sprintf(Unify_rssi1,"RSSI = %X\n\r",RSSI);
	HAL_UART_Transmit(&huart1,(uint8_t*)Unify_rssi1,10,100);*/

	Unify_sql = SPI_Read_HalfWord(AUXDATA2);
	Unify_squelch = Unify_sql & 0x0FFF;

	if(Unify_squelch < 900)
	{
		//sprintf(Unify_squelch1,"Squelch = %u\n\r",Unify_squelch);
		//HAL_UART_Transmit(&huart1,(uint8_t*)Unify_squelch1,15,100);
		Unify_AudioPAon();
	}
	else
	{
		//sprintf(Unify_squelch1,"Squelch = %u\n\r",Unify_squelch);
		//HAL_UART_Transmit(&huart1,(uint8_t*)Unify_squelch1,15,100);
		Unify_AudioPAoff();
	}
	return;
}
/***********AGC***********/
void Unify_AgcConfig(void)
{
	Spi_Write_HalfWord(POWERDOWN_CTRL,0xFFC4);	//power up everything
	Spi_Write_HalfWord(AUX_ADC_CTRL,0x0000); 	//Reset ADC
	//Spi_Write_HalfWord(AUX_ADC_CTRL,0x00BC);	 //set AuxADC1 input to AUX1, set averaging mode 1A19
	Spi_Write_HalfWord(AUX_ADC_CTRL,0x3A3C);	 //set AuxADC1 input to AUX1, set averaging mode 1A19
	Spi_Write_HalfWord(AUX_ADC1_THRESH_HI,0x0285); //set high threshold = $185
	Spi_Write_HalfWord(AUX_ADC1_THRESH_LO,0x0280);  //set low threshold = $180

	Spi_Write_HalfWord(AUX_ADC2_THRESH_HI,0x0285); //set high threshold = $185
	Spi_Write_HalfWord(AUX_ADC2_THRESH_LO,0x0280);  //set low threshold = $180
	return;
}

/***********************Program blocks**********************/
/*****BLOCK0******/
void Unify_Analogue_ProgBlock0(void)
{
	Spi_Write_HalfWord(PROG_ADR,0x0000);
	Unify_NextProgWrite(0x0000);	//Default value as per DS
	Unify_NextProgWrite(0x0000);	//Default value as per DS
	Unify_NextProgWrite(0x0000);	//Default value as per DS
	Unify_NextProgWrite(0x0000);	//Default value as per DS
	Unify_NextProgWrite(0x0000);	//Default value as per DS
	return;
}
/*******BLOCK2******/
void Unify_Analogue_ProgBlock2(void)
{
	/****Block2.0****/
	Spi_Write_HalfWord(PROG_ADR, 0x0200);
	//Unify_NextProgWrite(0x04C1);	 //changed from script
	Unify_NextProgWrite(0x0040); //Enabled I/Q Vad ,DCS sequence length 23 bit, MOD2 output mode Sub-Audio + In-band, MOD1 output mode Sub-Audio + In-band, Sub-Audio Tx Internal, Sub-Audio Rx External, CTCSS Phase Reversal disabled , Analogue Channel filter 25kHz
	Unify_NextProgWrite(0x6000); //Default Voice AGC Threshold as per DS
	Unify_NextProgWrite(0x00EA); //Default Voice AGC settings as per DS
	Unify_NextProgWrite(0x6000); //Default Voice Limiter levels +/- 75% full rail as per DS
	Unify_NextProgWrite(0x0002); //Default Internal CTCSS detector settings as per DS
	Unify_NextProgWrite(0x0C10); //Default
	Unify_NextProgWrite(0x0AE0); //Default
	Unify_NextProgWrite(0x0000); //Default
	return;
}
/******BLOCK6******/
void Unify_Analogue_ProgBlock6(void)
{
	/****Block6.0****/
	Spi_Write_HalfWord(PROG_ADR, 0x0600);
	Unify_NextProgWrite(0x0009); //8 HW AGC Disabled //Rx I/Q CMX994 mode , Tx Two-point mode , CMX994 I/Q AGC enabled (I/Q mode), HW AGC using AuxADC1 enabled (I/Q mode)
	Unify_NextProgWrite(0x8002); //SPI/C-BUS in Codec mode - SPI For AMBE 3000 operation
	Unify_NextProgWrite(0x0255); //CMX994E Variant Selected
	//Unify_NextProgWrite(0x0600); //Use both I and Q channels for energy sampling
	return;
}


/*******CMX994_CONFIGURATION********/
void Unify_config_cmx994(void)
{

	Unify_ProgWrite(GEN_CTRL_REG, 0x00C0); //Disable PLL - 80h to use ext LO and PLL 0080 default
	Unify_ProgWrite(RX_CTRL_REG, 0x0028);
	Unify_ProgWrite(OPT_CTRL_REG, 0x0080);  //Enabled enhanced mixer intermodulation mode in the receive path mixers
	Unify_ProgWrite(RX_GAIN_REG, 0x0008);	//Rx gain Reg. Gain = Max. 00 = Max gain in 100R mode and 08 = max gain in 50R mode
	Unify_ProgWrite(VCO_CTRL_REG, 0x0010);	// LO Input Enabled
	return;
}

void Unify_ProgWrite(uint16_t addr, uint16_t data)
{
	while(!(Unify_check_status & PROGBIT))
		Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for status
	Spi_Write_HalfWord(PROG_ADR, addr);
	Spi_Write_HalfWord(PROG_REG, data);
	return;
}

void Unify_NextProgWrite(uint16_t data)
{
	while(!(Unify_check_status & PROGBIT))
		Unify_check_status = SPI_Read_HalfWord(STATUS); //Wait for status

	Spi_Write_HalfWord(PROG_REG, data);
}


void Unify_AudioPAon(void)
{
	HAL_GPIO_WritePin(AUDIO_PA_SHDN_GPIO_Port,AUDIO_PA_SHDN_Pin,GPIO_PIN_RESET);
	return;
}

void Unify_AudioPAoff(void)
{
	HAL_GPIO_WritePin(AUDIO_PA_SHDN_GPIO_Port,AUDIO_PA_SHDN_Pin,GPIO_PIN_SET);
	return;
}


