/*
 * lcd.c
 *
 */

#include "u8g2/u8g2.h"
#include "lcd.h"
#include "stm32f1xx_hal.h"
#include "displaybitmaps.h"
#include <stdio.h>
#include "main.h"
#include <string.h>
#include <stdbool.h>
#include "sm.h"
#include "Analogue.h"
#include "i2c.h"
#include "Battery.h"
#include "radio.h"

#define RX_LOW

uint8_t battery_level;

uint8_t Bit_status = 0;

uint8_t Sq_button_status=0;
uint8_t pushButton_status=0;
uint32_t	keypos=21;
uint32_t msg_char_count;

extern bool ptt_mode,DMR_Mode; //mode;

extern uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

u8g2_t u8g2;
int squ_flag = 1;
int txpwr_flag = 1;
int disp_flag = 1;
uint32_t Analog_frq;
uint32_t Digital_frq;
uint32_t Tx_frq;
uint32_t Rx_frq;


void highlight_menu()
{
	u8g2_DrawBox(&u8g2,10,110,36,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_lcd_init()
{
	u8g2_Setup_ssd1327_i2c_midas_128x128_f(&u8g2, U8G2_R2,u8x8_byte_stm32_hw_i2c,u8x8_stm32_gpio_and_delay);

	// Initialize Display
	u8g2_InitDisplay(&u8g2);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
}

void unify_logo_screen()
{
	u8g2_FirstPage(&u8g2);
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2,1);
	u8g2_SetBitmapMode(&u8g2,1);

	//first screen
	u8g2_DrawBitmap(&u8g2, 0,0, bitmap_height, bitmap_width, logoUnify);

	u8g2_NextPage(&u8g2);
	u8g2_ClearBuffer(&u8g2);

}


//*******************************************************LCD API'S*************************************************************//
//callmenu message highlited screen
void unify_scr_mainmenu_screen2()
{

	Unify_Battery_Moniter();

	u8g2_DrawStr(&u8g2, 30, 45, "CALL");
	u8g2_DrawStr(&u8g2, 30, 60, "MESSAGE");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);

}



//individual call menu
void unify_scr_callmenu_screen3()
{
	Unify_Battery_Moniter();


	u8g2_DrawStr(&u8g2, 30, 45, "CALL1");
	u8g2_DrawStr(&u8g2, 30, 60, "CALL2");
	u8g2_DrawStr(&u8g2, 30, 75, "CALL3");
	u8g2_DrawStr(&u8g2, 30, 90, "GROUP CALL");
	u8g2_DrawStr(&u8g2, 30, 105, "ALL CALL");

	u8g2_DrawStr(&u8g2, 10, 120, "CALL");           //u8g2_DrawStr(&u8g2,horizontal, vertical, "ZONE");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);
}



//RADIO NUMBERS
void unify_indi_radio_screen4()
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 30, 45, "RADIO-1");
	u8g2_DrawStr(&u8g2, 30, 60, "RADIO-2");
	u8g2_DrawStr(&u8g2, 30, 75, "RADIO-3");
	u8g2_DrawStr(&u8g2, 30, 90, "RADIO-4");
}


void unify_scr_msgmenu_screen5()
{

	Unify_Battery_Moniter();
	u8g2_DrawStr(&u8g2, 30, 45, "IM FINE");
	u8g2_DrawStr(&u8g2, 30, 60, "GPS LOCATION");
	u8g2_DrawStr(&u8g2, 30, 75, "SET ID : 400");
	u8g2_DrawStr(&u8g2, 30, 90, "CUSTOM");

	u8g2_DrawStr(&u8g2, 10, 120, "SEND");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);
	u8g2_ClearBuffer(&u8g2);

}

//individual message menu
void unify_scr_msgmenu_screen6()
{

	u8g2_DrawStr(&u8g2, 30, 45, "INDIVIDUAL");
	u8g2_DrawStr(&u8g2, 30, 60, "GROUP");
	u8g2_DrawStr(&u8g2, 30, 75, "All MSG");

	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);

}




char* unify_pre_store_msg(int msg)
{
	if(msg == 1)
	{
		return "HELLO ITI";
	}
	else if(msg == 2)
	{
		return "HELLO UNIFY";
	}
}

void unify_pre_store_screen()
{
	unify_custom_indicators();

	char* iti= unify_pre_store_msg(1);
	char* unify=unify_pre_store_msg(2);

	u8g2_DrawStr(&u8g2, 30, 45,iti );
	u8g2_DrawStr(&u8g2, 30, 60, unify);
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);
}





void unify_next_page(void)
{
	u8g2_NextPage(&u8g2);
}


void clear_display()
{
	u8g2_ClearBuffer(&u8g2);
}

void unify_scr_opt()
{
	unify_custom_indicators();


	u8g2_DrawStr(&u8g2, 30, 45, "GPS");
	u8g2_DrawStr(&u8g2, 30, 60, "WIFI");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);
}

///////////////////////////////////////////////////////////////////////cursor API'S/////////////////////////////////////////////////////////////

void unify_call_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,32,32,14);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_msg_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,50,57,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);

}
void unify_radio_info_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,65,74,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_setting_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,80,57,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_indi_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,35,80,13);    //(u8g2, x, y, w, h);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}
void unify_group_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,50,80,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_allcall_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,65,65,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_dialcall_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,80,65,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_radio1_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,35,49,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_radio2_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,50,49,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);

}

void unify_radio3_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,65,49,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);

}

void unify_radio4_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,80,49,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);

}




void unify_pre_store_cursor1(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,35,60,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}


void unify_pre_store_cursor2()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,50,65,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}





void unify_gps_cursor()
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,35,33,12);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}
void unify_wifi_cursor(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,50,33,13);
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void unify_Radio_id()
{

	unify_custom_indicators();
	u8g2_DrawStr(&u8g2, 30, 45, "RADIO ID");
	u8g2_DrawStr(&u8g2, 30, 60, "00000001");
	u8g2_DrawStr(&u8g2, 10, 120, "DAIL");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);

}

void current_monirting()
{
	u8g2_ClearBuffer(&u8g2);
	unify_custom_indicators();
	u8g2_DrawStr(&u8g2, 30, 45, "Alert Current..!");
	u8g2_NextPage(&u8g2);
}


////////////////////////////////ROTARY API'S////////////////////////////////////////////

void ptt_display(int ptt)
{
	if(ptt==27)
	{
		char *a0[]={"PTT : ON "};

		u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
	}
	else
	{
		char *a0[]={"PTT : OFF "};
		u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
	}
}

void sql_display(int squl)
{

	if(squl==29)
	{
		if(squ_flag ==1)
		{
			squ_flag=1;
			char *a0[]={"Squelch : ON"};
			u8g2_DrawStr(&u8g2, 14, 89, a0[0]);
		}
		else
		{
			squ_flag=0;
			char *a0[]={"Squelch : OFF"};
			u8g2_DrawStr(&u8g2, 14, 89, a0[0]);
		}

	}
	else
	{
		if(squ_flag ==0)
		{
			squ_flag=0;
			char *a0[]={"Squelch : ON"};
			u8g2_DrawStr(&u8g2, 14, 89, a0[0]);
		}
		else
		{
			squ_flag=1;
			char *a0[]={"Squelch : OFF"};
			u8g2_DrawStr(&u8g2, 14, 89, a0[0]);
		}

	}


}

void txpwr_display(int txpwr)
{
	if(txpwr==33)
	{
		if(txpwr_flag ==1)
		{
			txpwr_flag=1;
			char *a0[]={"Tx Power : ON"};
			u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
		}
		else
		{
			txpwr_flag=0;
			char *a0[]={"Tx Power : OFF"};
			u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
		}

	}
	else
	{
		if(txpwr_flag ==0)
		{
			txpwr_flag=0;
			char *a0[]={"Tx Power : ON"};
			u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
		}
		else
		{
			txpwr_flag=1;
			char *a0[]={"Tx Power : OFF"};
			u8g2_DrawStr(&u8g2, 14, 74, a0[0]);
		}

	}
}



void unify_getFrq(int channel)
{
	char Freq[20];
	char Freq1[20];

	float i = 1000000;
	if(channel == 1 )
	{
		Analog_frq = 140000000;
		sprintf(Freq ,"Frq : %.6f MHz", Analog_frq/i);
		u8g2_DrawStr(&u8g2, 14, 30, Freq);

	}
	if(channel==2)
	{
		Digital_frq = 140000000;
		sprintf(Freq ,"Frq : %.6f MHz",Digital_frq/i);
		u8g2_DrawStr(&u8g2, 14, 30, Freq);
	}
	if(channel==3)
	{
		Tx_frq=140000000;
		Rx_frq=154225000;
		sprintf(Freq,"Tx_Frq : %dMHz",Tx_frq);
		sprintf(Freq1,"Rx_Frq :%dMHz",Rx_frq);
		u8g2_DrawStr(&u8g2,1,30,Freq);
		u8g2_DrawStr(&u8g2,1,44,Freq1);

	}

	return;
}

void unify_chType(int channel)
{
	if(channel == 1 )
	{
		char *a0[]={"MODE : ANALOG"};
		u8g2_DrawStr(&u8g2, 14, 59, a0[0]);
	}
	if(channel==2)
	{
		char *a0[]={"MODE : DMR"};
		u8g2_DrawStr(&u8g2, 14, 59, a0[0]);
	}
	if(channel==3)
	{
		char *a0[]={"MODE : ANALOG REP"};
		u8g2_DrawStr(&u8g2, 14, 59, a0[0]);
	}
	if(channel==4)
	{
		char *a0[]={"MODE : DMR REP"};
		u8g2_DrawStr(&u8g2, 14, 59, a0[0]);
	}
}


void unify_custom_indicators()
{
	u8g2_DrawXBMP(&u8g2, RF_SIGNAL_ROW,RF_SIGNAL_COL, RF_SIGNAL_WIDTH,  RF_SIGNAL_HEIGHT, signal5_20_17);	/*SIGNAL*/

	u8g2_DrawXBM(&u8g2, MESSAGE_BOX_COL, MESSAGE_BOX_ROW, MESSAGE_BOX_WIDTH, MESSAGE_BOX_HEIGHT, Msg17_17);	/*MSG*/
	u8g2_DrawXBM(&u8g2, BLUETOOTH_COL, BLUETOOTH_ROW,BLUETOOTH_SYMBOL_WIDTH,BLUETOOTH_SYMBOL_HEIGHT, Bluetooth_bits);    /*BLE*/
	u8g2_DrawXBM(&u8g2, WIFI_COL, WIFI_ROW, WIFI_SYMBOL_WIDTH, WIFI_SYMBOL_HEIGHT,wifi_Level4_bits);

}


void unify_home_screen(int ch,int squl,int ptt,int txpwr)
{
	u8g2_ClearBuffer(&u8g2);
	Unify_Battery_Moniter();
	highlight_menu();

	char channel[30];

	unify_getFrq(ch);


	if(ch==1 || ch==2){
		sprintf(channel , "Channel No :  %d",ch);
		u8g2_DrawStr(&u8g2, 14, 44, channel);
	}
	else if(ch==3 || ch==4)
	{
		u8g2_DrawStr(&u8g2, 14, 59, channel);
	}

	unify_chType(ch);
	ptt_display(ptt);


	u8g2_DrawStr(&u8g2, 10, 120, "MENU");

	u8g2_NextPage(&u8g2);
	u8g2_ClearBuffer(&u8g2);
}

uint8_t unify_PTT_switch(void)
{

	static uint8_t prev_ptt_pos,key;
	bool ptt_status;

	ptt_status = HAL_GPIO_ReadPin(PTT_IN_GPIO_Port,PTT_IN_Pin);

	if(ptt_status != prev_ptt_pos)
	{
		if(ptt_status)
			printf("H\n\r");
		else
			printf("L\n\r");
		prev_ptt_pos = ptt_status;
		if(ptt_mode && (!ptt_status) )
		{
			ptt_mode = false;
			printf("T\n\r");
			if(DMR_Mode)
				Radio.RadioState = START_DMR_TX;
			else
				Radio.RadioState = START_ANALOGUE_TX;
			key = 27;
		}
		if(ptt_status && !ptt_mode)
		{
			if(ptt_status)
				printf("H\n\r");
			else
				printf("L\n\r");
			ptt_mode = true;
			printf("R\n\r");
			if(DMR_Mode)
				Radio.Terminate = 1;
			else
			{
				Radio.Terminate = 1;
			}
			key = 28;
		}
	}
	return key;
}

void PTT_ON(void)
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "PTT Tx : Enabled...");
	u8g2_NextPage(&u8g2);
}

void PTT_OFF(void)
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "PTT Rx : Enabled...");

	u8g2_NextPage(&u8g2);
}
void Squelch_ON()
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "Squelch_ON");
	u8g2_NextPage(&u8g2);
}

void  Squelch_OFF()
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "Squelch_OFF");
	u8g2_NextPage(&u8g2);
}
void HL_Power_ON()
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "HL_Power_ON");
	u8g2_NextPage(&u8g2);
}

void  HL_Power_OFF()
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "HL_Power_OFF");
	u8g2_NextPage(&u8g2);
}

void Emergency_ON(void)
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "Emergency_ON");
	u8g2_NextPage(&u8g2);
}

void  Emergency_OFF(void)
{
	u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
	u8g2_DrawStr(&u8g2, 15, 50, "Emergency_OFF");
	u8g2_NextPage(&u8g2);
}

void right_cursor(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2,30,80,49,13);
	//u8g2_DrawBox(&u8g2,30,65,65,13)
	u8g2_SetFontMode(&u8g2,1);
	u8g2_SetDrawColor(&u8g2,2);
}

void battery_monitaring_levels(const uint8_t level[])
{
	u8g2_DrawXBM(&u8g2, BAT_SYMBOL_COL, BAT_SYMBOL_ROW,  BAT_SYMBOL_WIDTH, BAT_SYMBOL_HEIGHT, level);	/*BAttery*/
}



void unify_keypad_menu_screen()
{
	//unify_custom_indicators();
	//unify_custom_indicators1();
	Unify_Battery_Init();
	Unify_Battery_Moniter();
	u8g2_DrawStr(&u8g2, 15, 40, "Enter Message :");
	u8g2_DrawStr(&u8g2, 95, 120, "BACK");
	u8g2_NextPage(&u8g2);


	int current_key;

	//rotary_switch();
	char str[20];
	static int count=0;
	// key=KeyPad_WaitForKey(5000);
	//for(int i=15;i<1000;i+5)
	static int i=8,j=55;

	msg_char_count=0;
	do {
		current_key = unify_KeypadGetKey();
				//KeyPad_WaitForKeyGetChar(1000);

		if(current_key != 255)
		{
			if(count==14)
			{
				j=j+10;
				i=15;
				count=0;
			}
			if(current_key != '-1')
			{
				sprintf(str,"%c",current_key);

				// u8g2_ClearBuffer(&u8g2);
				u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
				if(msg_char_count<=64)
				{
					u8g2_DrawStr(&u8g2, i, j, str);
					msg_char_count++;
					//u8g2_ClearBuffer(&u8g2);
					u8g2_NextPage(&u8g2);
					count++;
					i=i+8;
				}
			}


		}
	}while(current_key != 61);
i=8;
j=55;
	msg_char_count=0;
}

