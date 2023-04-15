/*
 * rotary.c
 *
 *  Created on: Aug 2, 2022
 *      Author: dmr1
 */
#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "main.h"
#include "rotary.h"
#include "Analogue.h"
#include "function_image.h"
#include "ADF4351.h"
#include "Radio.h"
#include"lcd.h"

uint8_t key_PTT;
extern bool mode;
//uint8_t curr_pos =16,next_pos=16, ret_pos=0;
uint8_t prev_pos;
struct Channel_parameters Channel_pointer,CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15,CH16;

void print_Channel_parameters(struct Channel_parameters Channel_information)
{



	if(Channel_information.channel_type == 2 || Channel_information.channel_type == 3)
	{
		printf("Tx frequency: %f\n\r",Channel_information.txfrequency);
		printf("Rx frequency: %f\n\r",Channel_information.rxfrequency);
	}
	else
	{
		printf("frequency: %f\n\r",Channel_information.frequency);
	}
	printf("channel number:%d\n\r",Channel_information.channel_number);
	if(Channel_information.mode)
		printf("mode: Tx\n\r");
	else
		printf("mode: Rx\n\r");

	if(Channel_information.power)
		printf("power: High\n\r");
	else
		printf("power: Low\n\r");

	if(Channel_information.squelch == 1)
		printf("squelch: ON\n\r");
	else if (Channel_information.squelch == 0)
		printf("squelch: OFF\n\r");
	else
		printf("squelch: N/A\n\r");

	if(Channel_information.channel_type == 1)
		printf("channel_type: Digital\n\r");
	else if(Channel_information.channel_type == 2)
		printf("channel_type: Analog repeater\n\r");
	else if(Channel_information.channel_type == 3)
		printf("channel_type: Digital repeater\n\r");
	else
		printf("channel_type: Analog\n\r");
}

void unify_ChannelParameters(void)
{
	CH1.frequency = 140000000;
	CH1.channel_number = 1;
	CH1.mode = 0;
	CH1.power = 0;
	CH1.squelch = 1;
	CH1.channel_type = 0;

	CH2.frequency = 145000000;
	CH2.channel_number = 2;
	CH2.mode = 0;
	CH2.power = 0;
	CH2.squelch = 3;
	CH2.channel_type = 1;

	CH3.frequency = 170000000;
	CH3.txfrequency = 140000000;
	CH3.rxfrequency = 170000000;
	CH3.channel_number = 3;
	CH3.mode = 0;
	CH3.power = 0;
	CH3.squelch = 1;
	CH3.channel_type = 2;

	CH4.frequency = 170000000;
	CH4.txfrequency = 140000000;
	CH4.rxfrequency = 170000000;
	CH4.channel_number = 4;
	CH4.mode = 0;
	CH4.power = 0;
	CH4.squelch = 3;
	CH4.channel_type = 3;

	CH5.frequency = 140000000;
	CH5.channel_number = 5;
	CH5.mode = 0;
	CH5.power = 0;
	CH5.squelch = 1;
	CH5.channel_type = 0;

	CH6.frequency = 140000000;
	CH6.channel_number = 6;
	CH6.mode = 0;
	CH6.power = 0;
	CH6.squelch = 3;
	CH6.channel_type = 1;

	CH7.frequency = 170000000;
	CH7.txfrequency = 140000000;
	CH7.rxfrequency = 170000000;
	CH7.channel_number = 7;
	CH7.mode = 0;
	CH7.power = 0;
	CH7.squelch = 1;
	CH7.channel_type = 2;

	CH8.frequency = 170000000;
	CH8.txfrequency = 140000000;
	CH8.rxfrequency = 170000000;
	CH8.channel_number = 8;
	CH8.mode = 0;
	CH8.power = 0;
	CH8.squelch = 3;
	CH8.channel_type = 3;

	CH9.frequency = 140000000;
	CH9.channel_number = 9;
	CH9.mode = 0;
	CH9.power = 0;
	CH9.squelch = 1;
	CH9.channel_type = 0;

	CH10.frequency = 140000000;
	CH10.channel_number = 10;
	CH10.mode = 0;
	CH10.power = 0;
	CH10.squelch = 3;
	CH10.channel_type = 1;

	CH11.frequency = 170000000;
	CH11.txfrequency = 140000000;
	CH11.rxfrequency = 170000000;
	CH11.channel_number = 11;
	CH11.mode = 0;
	CH11.power = 0;
	CH11.squelch = 1;
	CH11.channel_type = 2;

	CH12.frequency = 170000000;
	CH12.txfrequency = 140000000;
	CH12.rxfrequency = 170000000;
	CH12.channel_number = 12;
	CH12.mode = 0;
	CH12.power = 0;
	CH12.squelch = 3;
	CH12.channel_type = 3;

	CH13.frequency = 140000000;
	CH13.channel_number = 13;
	CH13.mode = 0;
	CH13.power = 0;
	CH13.squelch = 1;
	CH13.channel_type = 0;

	CH14.frequency = 140000000;
	CH14.channel_number = 14;
	CH14.mode = 0;
	CH14.power = 0;
	CH14.squelch = 3;
	CH14.channel_type = 1;

	CH15.frequency = 170000000;
	CH15.txfrequency = 140000000;
	CH15.rxfrequency = 170000000;
	CH15.channel_number = 15;
	CH15.mode = 0;
	CH15.power = 0;
	CH15.squelch = 1;
	CH15.channel_type = 2;

	CH16.frequency = 170000000;
	CH16.txfrequency = 140000000;
	CH16.rxfrequency = 170000000;
	CH16.channel_number = 16;
	CH16.mode = 0;
	CH16.power = 0;
	CH16.squelch = 3;
	CH16.channel_type = 3;

}

/*
double frequency;
	double txfrequency;
	double rxfrequency;
	uint8_t channel_number;
	uint8_t mode;
	uint8_t power;
	uint8_t squelch;
	uint8_t channel_type;
*/



void unify_lcd_param(double frequency,double txfrequency,double rxfrequency,uint8_t channel_number,uint8_t mode,uint8_t power,uint8_t squelch,uint8_t channel_type)
{

	//unify_ChannelParameters();




}

void rotary_information(uint8_t status)
{
	double calcfreq;
	switch(status)
	{
	case 1:
		Channel_pointer = CH1;
		//unify_switch_case1(key_PTT);

		print_Channel_parameters(Channel_pointer);
		break;
	case 2:
		Channel_pointer = CH2;
		//unify_switch_case2(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;
	case 3:
		Channel_pointer = CH3;
		//unify_switch_case3(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;
	case 4:
		Channel_pointer = CH4;
		//unify_switch_case4(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 5:
		Channel_pointer = CH5;
		//unify_switch_case5(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 6:
		Channel_pointer = CH6;
		//unify_switch_case6(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 7:
		Channel_pointer = CH7;
		//unify_switch_case7(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 8:
		Channel_pointer = CH8;
		//unify_switch_case8(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 9:
		Channel_pointer = CH9;
		//unify_switch_case9(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 10:
		Channel_pointer = CH10;
		//unify_switch_case10(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 11:
		Channel_pointer = CH11;
		//unify_switch_case11(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 12:
		Channel_pointer = CH12;
		//unify_switch_case12(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 13:
		Channel_pointer = CH13;
		//unify_switch_case13(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 14:
		Channel_pointer = CH14;
		//unify_switch_case14(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 15:
		Channel_pointer = CH15;
		//unify_switch_case15(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;

	case 16:
		Channel_pointer = CH16;
		//unify_switch_case16(key_PTT);
		print_Channel_parameters(Channel_pointer);
		break;
	}

	//UpdateFrequencyRegisters(Channel_pointer.frequency*2, 38400000.0, 1000, 1, 1, &calcfreq);
	UpdateFrequencyRegisters(Channel_pointer.frequency, 38400000.0, 1000, 1, 1, &calcfreq);
	PLL_Sync();
	PLL_LD_Check(1000); //HAL_GPIO_ReadPin(LOCK_DETECT_GPIO_Port, LOCK_DETECT_Pin);

	if(Channel_pointer.channel_type == 1 || Channel_pointer.channel_type == 3)
	{
		printf("Loading HH in DMR mode\n\r");
		Unify_AudioPAon();
		mode = true;
		LoadFI(mode);
		InitFI2DMR();
	}
	else
	{
		printf("Loading HH in FM mode\n\r");
		mode = false;
		LoadFI(mode);
		Unify_Start_Analogue();
	}
}
uint8_t unify_RotaryGetKey(void)
{
	uint8_t bitStatus = 0,bitStatus1 = 0,bitStatus2 = 0,bitStatus3 = 0,Bit_status;

	//uint32_t count = 0xFFFFF;
	//while(count--);
	bitStatus3 = HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_3);
	bitStatus2 = HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_4);
	bitStatus1 = HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_5);
	bitStatus = HAL_GPIO_ReadPin (GPIOE, GPIO_PIN_6);
	Bit_status=((bitStatus3)|(bitStatus2<<1)|(bitStatus1<<2)|( bitStatus<<3 ));
	//printf("rotary Bit_status: %d\n\r",Bit_status);
	switch(Bit_status)
	{
	case 0:
		Bit_status = 26;
		break;
	case 1:
		Bit_status = 27;
		break;
	case 2:
		Bit_status = 25;
		break;
	case 3:
		Bit_status = 24;
		break;
	case 4:
		Bit_status = 29;
		break;
	case 5:
		Bit_status = 28;
		break;
	case 6:
		Bit_status = 30;
		break;
	case 7:
		Bit_status = 31;
		break;
	case 8:
		Bit_status = 21;
		break;
	case 9:
		Bit_status = 20;
		break;
	case 10:
		Bit_status = 22;
		break;
	case 11:
		Bit_status = 23;
		break;
	case 12:
		Bit_status = 18;
		break;
	case 13:
		Bit_status = 19;
		break;
	case 14:
		Bit_status = 17;
		break;
	case 15:
		Bit_status = 16;
		break;
	}

	/*if(prev_pos == Bit_status)
	{
		//printf("NULL\n\r");
		return 0x00;
	}
	else
	{
		//printf("rotary position: %d\n\r",Bit_status);
		prev_pos = Bit_status;
		return (Bit_status - 15);
	}*/
	return (Bit_status - 15);

}



