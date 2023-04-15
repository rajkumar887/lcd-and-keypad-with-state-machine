#include "sm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <keypad5x4-scanning.h>

//#include"KeyPad.h"
#include "Queue.h"
#include "lcd.h"
#include "rotary.h"
//#include "ais2ih.h"

extern uint16_t VoiceHeader[12];

int32_t mmifd = -1;
uint8_t mmi_sm_should_run = 0;
state_t* init_state;

uint8_t key_rotary;
extern uint8_t key_PTT;
uint8_t key_squelch;
uint8_t key_HL_power;

st_t* home_st;
st_t* main_st;
st_t* call_menu_st;
st_t* msg_menu_st;
st_t* radio_menu_st;
st_t* radio_menu_msg_st;
st_t* call_id_st;
st_t* number_menu_st;
st_t* gps_menu_st;
st_t* opt_menu_st;
st_t* pre_store_st;



st_t* installation_st;

/* MAIN_MENU PANNEL */
//st_t* settings_st;
//st_t* contact_st; 
//st_t* callregister_st;
//st_t* messages_st;

/*--------------------------------------------------------
//home register state entry function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _home_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	printf("home entry called\n\r");
	states_t ret = HOME_MENU;					/* returns to homes state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < HOME_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	//unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
	/* Displaying the selection on LCD according to the
		 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		break;
	case 2:

		break;
	case 3:

		break;

	}
	out:
	/* save the current state information as home state */
	info->current_state = HOME_MENU;
	return ret;
}

/*--------------------------------------------------------
//home state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _home_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = HOME_MENU;			/* rturns to home menu state */
	printf("home exit called\n\r");
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as homemenu state */
	info->previous_state = HOME_MENU;
	return ret;
}

/*--------------------------------------------------------
//home state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _home_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{			
	st_t* state = (st_t*)state_info;
	states_t ret = HOME_MENU;				/* returns to home menu state */
	printf("home action called\n\r");
	printf("event %x\n\r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}

	/* Displaying the selection on LCD according to the
					 row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		//unify_home_menu_screen1();
		ret = HOME_MENU;
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU;		/* when main menu key is pressed
															 it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		ret = HOME_MENU;
	}break;
	case UP_KEY:
	{
		printf("upkey pressed\n\r");
		ret = HOME_MENU;
	}break;
	case ENTER_KEY:

	{
		printf("enter key is pressed\n\r");
		state->row_idx=0 ;
		state->idx=1;
		printf("Selected option: %d\n\r", state->row_idx+ state->idx);
		switch(state->row_idx+ state->idx)
		{
		case 1:
			ret = MAIN_MENU;		/* returns main menu state */
			break;
		}
	}
	break;


	//unify_call_cursor();


#if 1
	case ROT_KEY1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		//init_function();
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY2:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY3:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY4:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY5:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY6:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY7:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY8:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY9:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY10:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY11:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY12:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY13:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY14:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY15:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY16:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
#endif

	/*case RIGHT_KEY:
		{
			//unify_switch_case1();

			right_cursor();
		  //unify_scr_mainmenu_screen2();
			break;
		}*/

	case LEFT_KEY:
	{
		//left_cursor();
		//clear_display();
		highlight_menu();
		break;
	}


	case PTT_KEY1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;

	}
	//clear_display();

	case PTT_KEY0:
	{

		/*PTT_ON();
		HAL_Delay(500);
		clear_display(); */
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		//	unify_switch_case1();

		//unify_next_page();
		//unify_call_cursor();
		//unify_scr_mainmenu_screen2();
		//	ret = MAIN_MENU;
		break;

	}
	//clear_display();
	case SQUELCH_KEY0:
	{
		/*Squelch_OFF();
		clear_display(); */
		//unify_switch_case1();
		//HAL_Delay(500);
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		//unify_next_page();
		break;

	}
	//clear_display();
	case SQUELCH_KEY1:
	{
		/*Squelch_ON();
		clear_display(); */
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		//unify_switch_case1();
		//HAL_Delay(500);

		//unify_next_page();
		break;

	}
	//	clear_display();
	case HL_POWER0:

	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case HL_POWER1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}


	case EMERGENCY_KEY0:
	{
		Emergency_OFF();
		HAL_Delay(1000);
		clear_display();
		break;
	}

	case EMERGENCY_KEY1:
	{
		Emergency_ON();
		HAL_Delay(1000);
		clear_display();
		break;
	}

	default:
		break;
	}
	out:
	return ret;
}

/*--------------------------------------------------------
//main menu state entry function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_main_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = MAIN_MENU;					/* returns ti main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the 
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_call_cursor();
		unify_scr_mainmenu_screen2();
		break;

	}


	out:
	/* save the current state information as main menu state */
	info->current_state = MAIN_MENU;
	return ret;
}

/*--------------------------------------------------------
//main menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_main_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = MAIN_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	//info->previous_state = MAIN_MENU;
	info->previous_state=HOME_MENU;
	return ret;
}

/*--------------------------------------------------------
//main menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_main_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	sm_t* info = (sm_t*)sm_info;
	states_t ret = MAIN_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the 
	row_idx of the state machine */
	switch (msg) 
	{
	case BACK_MENU_KEY:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;    /* when back menu key is pressed */
		break;
	}
	break;

	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed */

	}
	break;

	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_call_cursor();
			unify_scr_mainmenu_screen2();
			break;
		case 2:
			unify_msg_cursor();
			unify_scr_mainmenu_screen2();
			break;
		case 3:

			//clear_display();
			//unify_radio_info_cursor();
			//unify_scr_mainmenu_screen2();

			//unify_scr_mainmenu_screen2();

			break;
		case 4:
			//	unify_setting_cursor();
			//	unify_scr_mainmenu_screen2();
			break;
		case 5:
			//  right_cursor();
			//   unify_scr_mainmenu_screen2();
			break;
		}
	}
	break;

	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_call_cursor();
			unify_scr_mainmenu_screen2();
			break;
		case 2:
			unify_msg_cursor();
			unify_scr_mainmenu_screen2();
			break;
		case 3:
			//clear_display();
			//unify_radio_info_cursor();
			//	unify_scr_mainmenu_screen2();
			break;
		case 4:
			//	unify_setting_cursor();
			//	unify_scr_mainmenu_screen2();
			break;
		case 5:
			//	right_cursor();
			//	unify_scr_mainmenu_screen2();
			break;
		}
	}
	break;

	case ENTER_KEY:
	{
		printf("enter key is pressed\n\r");

		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			ret = CALL_MENU;
			break;
		case 2:
			ret=MESSAGE_MENU;  //return to messages state
			break;
		case 3:
			ret=OPT_MENU; // return to opt state
			break;
		case 4:
			unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
			ret=HOME_MENU; // return to home menu state
			break;
		}
	}
	break;

	case RIGHT_KEY:
	{

		right_cursor();
		unify_scr_mainmenu_screen2();

		/*switch (state->row_idx)
		{
		//if(state->row_idx==3)
	//	{
	case ENTER_KEY:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
		} when back menu key is pressed
		//}*/



		//}
	}

	break;

	case LEFT_KEY:
	{
		clear_display();
		highlight_menu();
		unify_scr_mainmenu_screen2();
		break;
	}
	break;

	case PTT_KEY1:
	{
		PTT_OFF();
		HAL_Delay(100);
		clear_display();
		break;
	}
	break;

	case PTT_KEY0:
	{

		PTT_ON();
		HAL_Delay(1000);
		clear_display();
		break;
	}
	break;

	default:
		break;
	}
	out:
	return ret;
}





/////////////////////////////////individual call menu/////////////////////////////////////////////
states_t _mmi_call_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = MAIN_MENU;					/* returns ti main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_indi_cursor();
		unify_scr_callmenu_screen3();
		VoiceHeader[3] = 0x00;
		VoiceHeader[4] = 0x00;
		VoiceHeader[5] = 0x01;
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;

	case 2:

		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = CALL_MENU;
	return ret;
}

/*--------------------------------------------------------
//call menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_call_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = CALL_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = CALL_MENU;
	return ret;
}

/*--------------------------------------------------------
//call menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_call_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)     //individual menu
{
	st_t* state = (st_t*)state_info;
	states_t ret = CALL_MENU;				/* returns to call menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = MAIN_MENU;    /* when back menu key is pressed
			//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=5)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_indi_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x01;
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_group_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x02;
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			unify_allcall_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x03;
			//_select_callregister();
			break;
		case 4:
			unify_dialcall_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x0FF;
			VoiceHeader[4] = 0xFC;
			VoiceHeader[5] = 0xDF;
			//_select_settings();
			break;
		case 5:
			right_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x0FF;
			VoiceHeader[4] = 0xFF;
			VoiceHeader[5] = 0xFF;
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=5;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_indi_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x01;
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_group_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x02;
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			unify_allcall_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x00;
			VoiceHeader[4] = 0x00;
			VoiceHeader[5] = 0x03;
			//_select_callregister();
			break;
		case 4:
			unify_dialcall_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x0FF;
			VoiceHeader[4] = 0xFC;
			VoiceHeader[5] = 0xDF;
			//_select_settings();
			break;
		case 5:
			right_cursor();
			unify_scr_callmenu_screen3();
			VoiceHeader[3] = 0x0FF;
			VoiceHeader[4] = 0xFF;
			VoiceHeader[5] = 0xFF;
			break;
		}

	}break;
	case ENTER_KEY:
	{
		/*unify_indi_cursor();
		unify_scr_callmenu_screen3();*/
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			ret = RADIO_MENU;
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		case 5:
			ret = MAIN_MENU;
			break;
		}
	}

	case RIGHT_KEY:
	{
		right_cursor();
		unify_scr_callmenu_screen3();
		break;

	}

	case LEFT_KEY:
	{
		highlight_menu();
		unify_scr_callmenu_screen3();
		break;

	}

	break;
	default:
		break;
	}
	out:
	return ret;
}

#if 1

////////////////////////////individual call radio menu///////////////////////////////////////////////
states_t _mmi_radio_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = RADIO_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < RADIO_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_radio1_cursor();
		unify_indi_radio_screen4();
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;


	case 2:
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = RADIO_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_radio_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = CALL_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = CALL_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_radio_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = RADIO_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = CALL_MENU;    /* when back menu key is pressed
			//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_radio1_cursor();
			unify_indi_radio_screen4();

			//_select_contacts();
			break;

		case 2:
			unify_radio2_cursor();
			unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			unify_radio3_cursor();
			unify_indi_radio_screen4();
			//_select_callregister();
			break;
		case 4:
			unify_radio4_cursor();
			unify_indi_radio_screen4();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_radio1_cursor();
			unify_indi_radio_screen4();
			//_select_contacts();
			break;

		case 2:
			unify_radio2_cursor();
			unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			unify_radio3_cursor();
			unify_indi_radio_screen4();

			//_select_callregister();
			break;
		case 4:
			unify_radio4_cursor();
			unify_indi_radio_screen4();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			ret=CALL_ID_MENU;
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	} break;
	default:
		break;
	}
	out:
	return ret;
}


////////////////////////////CALL_ID menu///////////////////////////////////////////////
states_t _mmi_call_id_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = CALL_ID_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < CALL_ID_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_Radio_id();
		/*unify_radio1_cursor();
		unify_indi_radio_screen4();*/
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;


	case 2:
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = CALL_ID_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_call_id_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = CALL_ID_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = CALL_ID_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_call_id_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = CALL_ID_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = RADIO_MENU;    /* when back menu key is pressed
			//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			/*unify_radio1_cursor();
			unify_indi_radio_screen4();*/

			//_select_contacts();
			break;

		case 2:
			/*unify_radio2_cursor();
			unify_indi_radio_screen4();*/
			//_select_messages();
			break;
		case 3:
			/*unify_radio3_cursor();
			unify_indi_radio_screen4();*/
			//_select_callregister();
			break;
		case 4:
			/*unify_radio4_cursor();
			unify_indi_radio_screen4();*/
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			/*unify_radio1_cursor();
			unify_indi_radio_screen4();*/
			//_select_contacts();
			break;

		case 2:
			/*unify_radio2_cursor();
			unify_indi_radio_screen4();*/
			//_select_messages();
			break;
		case 3:
			/*unify_radio3_cursor();
			unify_indi_radio_screen4();
			 */
			//_select_callregister();
			break;
		case 4:
			/*unify_radio4_cursor();
			unify_indi_radio_screen4();*/
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:

			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	} break;
	default:
		break;
	}
	out:
	return ret;
}



#endif



/*--------------------------------------------------------
//message menu state entry function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/

//////////////////////////////////////////individual message menu/////////////////////////////
states_t _mmi_message_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = MESSAGE_MENU;					/* returns ti main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_indi_cursor();
		//	unify_msg_cursor();
		//ptt_display(ptt);
		unify_scr_msgmenu_screen5();
		//_select_contacts();
		break;


	case 2:

		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = MESSAGE_MENU;
	return ret;
}

/*--------------------------------------------------------
//call menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_message_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = MESSAGE_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = MAIN_MENU;
	return ret;
}

/*--------------------------------------------------------
//call menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_message_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = MESSAGE_MENU;				/* returns to call menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		unify_msg_cursor();
		//unify_call_cursor();
		unify_scr_mainmenu_screen2();
		ret = MAIN_MENU;    //when back menu key is pressed

	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU;		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_indi_cursor();
			unify_scr_msgmenu_screen5();
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_group_cursor();
			unify_scr_msgmenu_screen5();
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			unify_allcall_cursor();
			unify_scr_msgmenu_screen5();
			//_select_callregister();
			break;
		case 4:
			right_cursor();
			//unify_allcall_cursor();
			unify_scr_msgmenu_screen5();
			//unify_dialcall_cursor();
			//unify_scr_msgmenu_screen5();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_indi_cursor();
			unify_scr_msgmenu_screen5();
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_group_cursor();
			unify_scr_msgmenu_screen5();
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			unify_allcall_cursor();
			unify_scr_msgmenu_screen5();
			//_select_callregister();
			break;
		case 4:
			right_cursor();
			//unify_allcall_cursor();
			unify_scr_msgmenu_screen5();
			//unify_dialcall_cursor();
			//unify_scr_msgmenu_screen5();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		/*unify_indi_cursor();
		unify_scr_callmenu_screen3();*/
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			break;
		case 2:
			ret = RADIO_MSG_MENU;
			break;
		case 3:
			//ret =NUMBER_MSG_MENU;
			break;
		case 4:
			//unify_allcall_cursor();
			//unify_scr_msgmenu_screen5();
			ret = NUMBER_MSG_MENU;
			break;
		}
	}
#if 0

	case ROT_KEY1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY2:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY3:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY4:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY5:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY6:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY7:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY8:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY9:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY10:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY11:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY12:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY13:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY14:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY15:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY16:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
#endif

	/*case RIGHT_KEY:
			{
				right_cursor();
				unify_scr_msgmenu_screen5();
				break;

			}

			case LEFT_KEY:
			{
				clear_display();
				highlight_menu();
				unify_scr_msgmenu_screen5();
				break;

			}*/





	break;
	default:
		break;
	}
	out:
	return ret;
}












////////////////////////////radio msg menu///////////////////////////////////////////////
states_t _mmi_radio_msg_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = RADIO_MSG_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MESSAGE_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}

	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		//unify_indi_cursor();
		//	unify_scr_msgmenu_screen6();
		unify_scr_msgmenu_screen5();
		break;


	case 2:
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = RADIO_MSG_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_radio_msg_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = RADIO_MSG_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = MESSAGE_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_radio_msg_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = RADIO_MSG_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = MESSAGE_MENU;    /* when back menu key is pressed
			//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_indi_cursor();
			//unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();
			break;

		case 2:
			//unify_group_cursor();
			//unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();
			break;
		case 3:
			//unify_allcall_cursor();
			//unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();
			break;
		case 4:
			//right_cursor();
			//unify_scr_msgmenu_screen6();
			//unify_scr_msgmenu_screen5();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_indi_cursor();
			//unify_scr_msgmenu_screen6();

			unify_scr_msgmenu_screen5();
			break;

		case 2:
			//unify_group_cursor();
			//unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();
			break;
		case 3:
			//unify_allcall_cursor();
			//	unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();

			break;
		case 4:
			//	right_cursor();
			//unify_scr_msgmenu_screen6();
			unify_scr_msgmenu_screen5();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			//ret=NUMBER_MSG_MENU;
			ret=PRE_MENU;
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = MESSAGE_MENU;			/* returns to settings state */
			ret=NUMBER_MSG_MENU;
			break;
		}
	}
#if 0
	case ROT_KEY1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY2:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY3:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY4:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY5:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY6:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY7:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY8:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY9:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY10:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY11:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY12:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY13:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY14:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY15:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY16:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
#endif

	break;
	default:
		break;
	}
	out:
	return ret;
}

////////////////////////////pre-stored msg///////////////////////////////////////////////
states_t _mmi_pre_store_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret =PRE_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < PRE_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}

	//	unify_home_menu_screen1(msg);
	//	unify_next_page();
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_pre_store_cursor1();
		unify_pre_store_screen();
		/*unify_home_menu_screen1();
		unify_next_page();*/

		//unify_radio1_cursor();
		//unify_indi_radio_screen4();
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;


	case 2:
		/*unify_home_menu_screen1();
				unify_next_page();*/
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = PRE_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_pre_store_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = PRE_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = PRE_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_pre_store_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = PRE_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = RADIO_MSG_MENU;    /* when back menu key is pressed
			//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_pre_store_cursor1();
			unify_pre_store_screen();
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();

			//_select_contacts();
			break;

		case 2:
			unify_pre_store_cursor2();
			unify_pre_store_screen();
			//	unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			right_cursor();
			unify_pre_store_screen();
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();
			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_pre_store_cursor1();
			unify_pre_store_screen();
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();
			//_select_contacts();
			break;

		case 2:
			unify_pre_store_cursor2();
			unify_pre_store_screen();
			//unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			right_cursor();
			unify_pre_store_screen();
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();

			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			unify_pre_store_cursor1();
			unify_pre_store_screen();
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = MAIN_MENU;
			ret =RADIO_MSG_MENU;
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	}

#if 0
	case ROT_KEY1:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY2:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY3:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY4:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY5:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY6:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY7:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY8:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY9:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY10:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY11:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}

	case ROT_KEY12:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY13:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY14:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		break;
	}

	case ROT_KEY15:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
	case ROT_KEY16:
	{
		unify_home_screen(key_rotary,key_squelch,key_PTT,key_HL_power);
		ret = HOME_MENU;
		break;
	}
#endif

	break;
	default:
		break;

	}
	out:
	return ret;
}
//break;
//}
//}



////////////////////////////number menu///////////////////////////////////////////////
states_t _mmi_number_msg_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)

{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret =NUMBER_MSG_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < NUMBER_MSG_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	unify_keypad_menu_screen();
	unify_next_page();
	/* Displaying the selection on LCD according to the
	 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		/*unify_home_menu_screen1();
		unify_next_page();*/

		//unify_radio1_cursor();
		//unify_indi_radio_screen4();
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;


	case 2:
		/*unify_home_menu_screen1();
				unify_next_page();*/
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = NUMBER_MSG_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state exit function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_number_msg_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = MESSAGE_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;

	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = MESSAGE_MENU;
	return ret;
}

/*--------------------------------------------------------
//radio menu state action function
//param1 - state machine information
//param2 - state information
//param3 - event
----------------------------------------------------------*/
states_t _mmi_number_msg_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)

{
	st_t* state = (st_t*)state_info;
	states_t ret = NUMBER_MSG_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
	row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret =MESSAGE_MENU;
		//ret = RADIO_MSG_MENU;    /* when back menu key is pressed
		//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
			it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();

			//_select_contacts();
			break;

		case 2:
			//	unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();
			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
			depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();
			//_select_contacts();
			break;

		case 2:
			//unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();

			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
			depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:

			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	}

	break;
	default:
		break;

	}
	out:
	return ret;
}
//break;
//}
//}


////////////////////////////number menu///////////////////////////////////////////////
states_t _mmi_gps_msg_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret =MAIN_MENU;					/* returns  radio menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	//unify_home_menu_screen1(msg);
	//unify_next_page();
	/* Displaying the selection on LCD according to the
		 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_custom_indicators();
		//INA260_Battery_Init();
		/*unify_home_menu_screen1();
			unify_next_page();*/

		//unify_radio1_cursor();
		//unify_indi_radio_screen4();
		//unify_scr_indimenu_call_screen3();
		//_select_contacts();
		break;


	case 2:
		/*unify_home_menu_screen1();
					unify_next_page();*/
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = GPS_MENU;
	return ret;
}

/*--------------------------------------------------------
	//radio menu state exit function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_gps_msg_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = GPS_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = GPS_MENU;
	return ret;
}

/*--------------------------------------------------------
	//radio menu state action function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_gps_msg_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = GPS_MENU;				/* returns to main menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
		row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = MAIN_MENU;    /* when back menu key is pressed
				//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU; 		/* when mainmenu key is pressed
				it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();

			//_select_contacts();
			break;

		case 2:
			//	unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();
			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			//unify_custom_indicators();
			//INA260_Battery_Init();
			//unify_radio1_cursor();
			//unify_indi_radio_screen4();
			//_select_contacts();
			break;

		case 2:
			//unify_radio2_cursor();
			//unify_indi_radio_screen4();
			//_select_messages();
			break;
		case 3:
			//unify_radio3_cursor();
			//unify_indi_radio_screen4();

			//_select_callregister();
			break;
		case 4:
			//unify_radio4_cursor();
			//unify_indi_radio_screen4();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		//unify_Radio_contacts_menu();
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
				depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			unify_next_page();
			unify_custom_indicators();
			//INA260_Battery_Init();



			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}

	}
	break;
	default:
		break;

	}
	out:
	return ret;
}


//////////////////////////////////////////opt message menu/////////////////////////////
states_t _mmi_opt_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = MAIN_MENU;					/* returns ti main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
		 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_gps_cursor();
		unify_scr_opt();
		//unify_custom_indicators();
		//INA260_Battery_Init();
		//unify_indi_cursor();
		//	unify_scr_msgmenu_screen5();
		//_select_contacts();
		break;


	case 2:

		//unify_scr_opt();
		//unify_indi_cursor();
		//unify_scr_msgmenu_screen5();
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = OPT_MENU;
	return ret;
}

/*--------------------------------------------------------
	//call menu state exit function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_opt_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = OPT_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = OPT_MENU;
	return ret;
}

/*--------------------------------------------------------
	//call menu state action function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_opt_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = OPT_MENU;				/* returns to call menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
		row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = MAIN_MENU;    /* when back menu key is pressed
			//ret=HOME_MENU;
				//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU;		/* when mainmenu key is pressed
				it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_gps_cursor();
			unify_scr_opt();
			//unify_indi_cursor();
			//unify_scr_msgmenu_screen5();
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_wifi_cursor();
			unify_scr_opt();
			//unify_group_cursor();
			//unify_scr_msgmenu_screen5();
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:


			//	unify_allcall_cursor();
			//	unify_scr_msgmenu_screen5();
			//_select_callregister();
			break;
		case 4:
			//unify_dialcall_cursor();
			//unify_scr_msgmenu_screen5();
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			unify_gps_cursor();
			unify_scr_opt();
			//unify_indi_cursor();
			//unify_scr_msgmenu_screen5();
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			unify_wifi_cursor();
			unify_scr_opt();
			//	unify_group_cursor();
			//	unify_scr_msgmenu_screen5();
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			//unify_allcall_cursor();
			//	unify_scr_msgmenu_screen5();
			//_select_callregister();
			break;
		case 4:
			//	unify_dialcall_cursor();
			//	unify_scr_msgmenu_screen5();
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{

		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
				depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:

			ret=GPS_MENU;
			//unify_next_page();
			/*unify_custom_indicators();
				INA260_Battery_Init();*/
			//	unify_next_page();
			//ret = RADIO_MSG_MENU;
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	}

	/*case RIGHT_KEY:
				{
					right_cursor();
					unify_scr_msgmenu_screen5();
					break;

				}

				case LEFT_KEY:
				{
					highlight_menu();
					unify_scr_msgmenu_screen5();
					break;

				}
	 */




	break;
	default:
		break;
	}
	out:
	return ret;
}

















#if 1

/*--------------------------------------------------------
	//gps menu state entry function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/

//////////////////////////////////////////gps menu/////////////////////////////
states_t _mmi_gps_menu_entry(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	st_t* state = (st_t*)*state_info;
	states_t ret = MAIN_MENU;					/* returns ti main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		ret = INVALID;
		printf("Invalid arguments \n\r");
		goto out;
	}
	/* save the previous state information into state machine */
	state->previous_state = info->previous_state;
	if(info->previous_state < MAIN_MENU )
	{
		state->row_idx = 1;
		state->idx=0;
	}
	/* Displaying the selection on LCD according to the
		 row_idx of the state machine */
	switch(state->row_idx)
	{
	case 1:
		unify_scr_opt();
		//unify_indi_cursor();
		//	unify_scr_msgmenu_screen5();
		//_select_contacts();
		break;


	case 2:
		//unify_indi_cursor();
		//unify_scr_msgmenu_screen5();
		//unify_scr_msgmenu_call();
		//_select_messages();
		break;
	}
	out:
	/* save the current state information as main menu state */
	info->current_state = GPS_MENU;
	return ret;
}

/*--------------------------------------------------------
	//call menu state exit function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_gps_menu_exit(uint8_t* sm_info, uint8_t **state_info, event_t msg)
{
	sm_t* info = (sm_t*)sm_info;
	states_t ret = GPS_MENU;				/* returns to main menu state */
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* clear the LCD screen */
	//FillRect(_LCD_INIT_X,_LCD_INIT_Y,_LCD_WIDTH,_LCD_HEIGHT,COLOR_WHITE);
	clear_display();
	out:
	/* save the previous state information as main menu state */
	info->previous_state = GPS_MENU;
	return ret;
}

/*--------------------------------------------------------
	//call menu state action function
	//param1 - state machine information
	//param2 - state information
	//param3 - event
	----------------------------------------------------------*/
states_t _mmi_gps_menu_action (uint8_t* sm_info, uint8_t *state_info, event_t msg)
{
	st_t* state = (st_t*)state_info;
	states_t ret = GPS_MENU;				/* returns to call menu state */
	printf("MAIN ACTION called\n\r");
	printf("event %x\n\r \r",msg);
	if (sm_info == UNIFY_NULL || msg == UNIFY_NULL)
	{
		printf("Invalid arguments \n\r");
		ret = INVALID;
		goto out;
	}
	/* Displaying the selection on LCD according to the
		row_idx of the state machine */
	switch (msg)
	{
	case BACK_MENU_KEY:
	{
		ret = MAIN_MENU;    /* when back menu key is pressed
			//ret=HOME_MENU;
				//it returns to home menu state */
	}break;
	case MAIN_MENU_KEY:
	{
		ret = HOME_MENU;		/* when mainmenu key is pressed
				it returns to home menu state */
	}break;
	case DOWN_KEY:
	{
		printf("Downkey pressed\n\r");
		if(state->row_idx>=4)
		{
			state->row_idx=1;
		}
		else
		{
			state->row_idx++;
		}
		printf(" THE COUNT VALUE %d",state->row_idx);
		/* when down key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			/*unify_indi_cursor();
				unify_scr_msgmenu_screen5();*/
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			/*unify_group_cursor();
				unify_scr_msgmenu_screen5();*/
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			/*unify_allcall_cursor();
				unify_scr_msgmenu_screen5();*/
			//_select_callregister();
			break;
		case 4:
			/*unify_dialcall_cursor();
				unify_scr_msgmenu_screen5();*/
			//_select_settings();
			break;
		}

	}break;
	case UP_KEY:
	{
		printf("UP KEY pressed\n\r");
		if(state->row_idx<=1)
		{
			state->row_idx=4;
		}
		else
		{
			state->row_idx--;
		}
		/* when up key is pressed the selection on the LCD will be changed
				depending on the  row_idx count */
		switch(state->row_idx)
		{
		case 1:
			/*	unify_indi_cursor();
				unify_scr_msgmenu_screen5();*/
			//unify_scr_indimenu_call_screen3();
			//_select_contacts();
			break;

		case 2:
			/*unify_group_cursor();
				unify_scr_msgmenu_screen5();*/
			//unify_scr_msgmenu_call();
			//_select_messages();
			break;
		case 3:
			/*unify_allcall_cursor();
				unify_scr_msgmenu_screen5();*/
			//_select_callregister();
			break;
		case 4:
			/*unify_dialcall_cursor();
				unify_scr_msgmenu_screen5();*/
			//_select_settings();
			break;
		}
	}break;
	case ENTER_KEY:
	{
		/*unify_indi_cursor();
			unify_scr_callmenu_screen3();*/
		printf("enter key is pressed\n\r");
		printf(" COUNT VALUE %d \n\r",state->row_idx);
		/* when enter key is pressed the selection on the LCD will be changed
				depending on the  row_idx count and returns to corresponding state */
		switch(state->row_idx)
		{
		case 1:
			//ret = RADIO_MSG_MENU;
			//ret = CONTACTS;			/* returns to contacts state */
			break;
		case 2:
			//ret = MESSAGES;			/* returns to messages state */
			break;
		case 3:
			//ret = CALL_REGISTER;	/* returns to call register state */
			break;
		case 4:
			//ret = SETTINGS;			/* returns to settings state */
			break;
		}
	} break;
	default:
		break;
	}
	out:
	return ret;
}


#endif






uint8_t _idu_mmi_msg_send(uint8_t* msg)
{
	return 0;
}
/*-------------------------------------------------------------
//
// intialise the human machine interface intialisatioin
//
--------------------------------------------------------------*/
uint8_t  _idu_mmi_init(void)
{
	state_t* state;
	int ret = 0;
	//printf("%d\n\r", __LINE__);
	/* allocating the memory for home state during execution of program */


	/****************************************1 state starts*****************************************/

	home_st = (st_t *)malloc(sizeof(st_t));
	if (home_st == UNIFY_NULL)
	{
		printf("Memory allocation failed\n\r");
		goto out;
	}
	memset(home_st, 0, sizeof(st_t));
	home_st->row_idx = 2;
	/* adding the home menu state information into state machine */
	state = add_state_info (HOME_MENU,
			_home_entry,
			_home_action,
			_home_exit,
			(uint8_t*)home_st);
	printf(" home  STATE \n\r ");

	if (state == UNIFY_NULL)
	{
		printf("Adding state %d failed\n\r", HOME_MENU);
		ret = -1;
		goto out;
	}
	init_state = state;

	/*****************************************1 state ends*************************************************/


	/****************************************2 state starts*****************************************/

	/* allocating the memory for main menu state during execution of program */
	main_st = (st_t *)malloc(sizeof(st_t));
	if (main_st == UNIFY_NULL)
	{
		printf("Memory allocation failed\n\r");
		goto out;
	}
	memset(main_st, 0, sizeof(st_t));
	main_st->row_idx = 2;
	/* Adding  Main MENU state to state machine */
	state = add_state_info (MAIN_MENU,
			_mmi_main_entry,
			_mmi_main_action,
			_mmi_main_exit,						///let
			(uint8_t*)main_st);
	printf("MAIN_MENU STATE \n\r");

	/****************************************2 state ends*****************************************/



	/****************************************3 state starts*****************************************/
	/////////////////////////CALL MENU///////////////////////////////////////////

	/* allocating the memory for call menu state during execution of program */
	call_menu_st = (st_t *)malloc(sizeof(st_t));
	if (call_menu_st == UNIFY_NULL)
	{
		printf("Memory allocation failed\n\r");
		goto out;
	}
	memset(call_menu_st, 0, sizeof(st_t));
	call_menu_st->row_idx = 1;
	/* Adding  Main MENU state to state machine */
	state = add_state_info (CALL_MENU,
			_mmi_call_menu_entry,					//let
			_mmi_call_menu_action,
			_mmi_call_menu_exit,
			(uint8_t*)call_menu_st);
	printf("CALL_MENU STATE \n\r");

	/****************************************3 state ends*****************************************/


	/****************************************4 state starts*****************************************/

	///////////////////////////////msg menu///////////////////////
	msg_menu_st = (st_t *)malloc(sizeof(st_t));
	if (msg_menu_st == UNIFY_NULL)
	{
		printf("Memory allocation failed\n\r");
		goto out;
	}
	memset(msg_menu_st, 0, sizeof(st_t));
	msg_menu_st->row_idx = 1;
	// Adding  Main MENU state to state machine
	state = add_state_info (MESSAGE_MENU,				//let
			_mmi_message_menu_entry,
			_mmi_message_menu_action,
			_mmi_message_menu_exit,
			(uint8_t*)msg_menu_st);
	printf("MESSAGE_MENU STATE \n\r");


	/****************************************4 state ends*****************************************/


	/****************************************5 state starts*****************************************/

	///////////////////////////////NUMBER_MENU //////////////////////////////
		/* allocating the memory for radio menu state during execution of program */
		number_menu_st = (st_t *)malloc(sizeof(st_t));
		if (number_menu_st == UNIFY_NULL)
		{
			printf("Memory allocation failed\n\r");
			goto out;
		}
		memset(number_menu_st, 0, sizeof(st_t));
		number_menu_st->row_idx = 1;
		/* Adding  Main MENU state to state machine */
		state = add_state_info (NUMBER_MSG_MENU,
				_mmi_number_msg_menu_entry,
				_mmi_number_msg_menu_action,
				_mmi_number_msg_menu_exit,
				(uint8_t*)number_menu_st);
		printf("NUMBER_MSG_MENU STATE \n\r");

		/****************************************5 state ends*****************************************/


	if (state == UNIFY_NULL)
	{
		printf("Adding state %d failed\n\r", MAIN_MENU);
		ret = -1;
		goto out;
	}
	if (state == UNIFY_NULL)

		out:
		return ret;
}

int _idu_mmi_start(void)
{
	sm_t* sm_info;
	event_t event=(event_t)DOWN_KEY;
	int ret = 0;
	mmi_sm_should_run = 1;
	sm_info = (sm_t*) malloc (sizeof(sm_t));
	if (sm_info == NULL)
	{
		printf("Memory allocation failed\n\r");
		goto out;
	}
	mmifd=0;    
	sm_info->fd = mmifd;
	/* Trigger the state machine */
	ret = state_machine_run((uint8_t*)sm_info, init_state, event);
	if (ret == -1)
	{
		printf(" State Machine failed \n\r");
		goto out;
	}

	printf("End***\n\r");
	free(sm_info);
	out:
	return 0;
}


void _idu_mmi_stop(void)
{
}
void _idu_mmi_signal(int sig)
{
}

void _idu_mmi_exit(int sig)
{
	printf("MMI: Exit Invoked\n\r");
	mmi_sm_should_run = 0;

	del_state_info(MAIN_MENU);
	del_state_info(INSTALLATION);

	free(main_st);
	free(installation_st);
}
