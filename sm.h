
#ifndef SM_H
#define SM_H

#include <stdio.h>  
#include <string.h>  
#include <Queue.h>


typedef enum
{
	INVALID = -1,
	HOME_MENU = 0,
	MAIN_MENU = 1,
	CALL_MENU = 2,
	MESSAGE_MENU=3,
	RADIO_MENU=4,
	RADIO_MSG_MENU=5,
	CALL_ID_MENU=6,
	NUMBER_MSG_MENU=7,
	GPS_MENU=8,
	OPT_MENU=9,
	PRE_MENU=10,

	INSTALLATION,
	//SETTINGS     ,
	//MESSAGES    ,
	//CONTACTS     ,
	//CALL_REGISTER ,

	MAX_STATES /* keep it last */
}states_t;

typedef enum
{
	ZERO_KEY=0x30,

	ONE_KEY=0x31,

	ONE_KEY_a=0x61,

	ONE_KEY_b=0x62,

	ONE_KEY_c=0x63,


	TWO_KEY=0x32,

	THREE_KEY=0x33,

	FOUR_KEY=0x34,

	FIVE_KEY=0x35,

	SIX_KEY=0x36,

	SEVEN_KEY=0x37,

	EIGHT_KEY=0x38,

	NINE_KEY=0x39,

	ASTERISK_KEY=0x2A,

	NUMBER_KEY=0X23,


	UP_KEY = 0x55,  //U=85 decimal to hexadecimal(0x55)

	DOWN_KEY = 0x44,  //D=68 decimal to hexadecimal(0x44)

	BACK_MENU_KEY = 0x42,  //B=66 decimal to hexadecimal(0x42)

	MAIN_MENU_KEY = 0x4D,   //M=77 decimal to hexadecimal(0x4D)
	//MAIN_MENU_KEY = 0x45,

	ENTER_KEY = 0x45,     //E=69 decimal to hexadecimal(0x45)

	LEFT_KEY = 0x4C,     //L=76  decimal to hexadecimal(0x4C)

	RIGHT_KEY = 0x52,    //R=82  decimal to hexadecimal(0x52)

	CALL_KEY = 0x43,     //C=67  decimal to hexadecimal(0x43)





	ROT_KEY1=0x01,         //rotary key 11

	ROT_KEY2=0x02,			//rotary key 12

	ROT_KEY3=0x03,			//rotary key 13

	ROT_KEY4=0x04,         //rotary key 14

	ROT_KEY5=0x05,			//rotary key 15

	ROT_KEY6=0x06,			//rotary key 16

	ROT_KEY7=0x07,         //rotary key 17

	ROT_KEY8=0x08,			//rotary key 18

	ROT_KEY9=0x09,			//rotary key 19

	ROT_KEY10=0x0A,         //rotary key 20

	ROT_KEY11=0x0B,			//rotary key 21

	ROT_KEY12=0x0C,			//rotary key 22

	ROT_KEY13=0x0D,         //rotary key 23

	ROT_KEY14=0x0E,			//rotary key 24

	ROT_KEY15=0x0F,			//rotary key 25

	ROT_KEY16=0x10,			//rotary key 26

	PTT_KEY1=0x1B,			//PTT key 27

	PTT_KEY0=0x1C,      	 //PTT key 28

	SQUELCH_KEY0=0x1D,		//SQUELCH_KEY 29

	SQUELCH_KEY1=0x1E,      //SQUELCH_KEY 30

	EMERGENCY_KEY0=0x1F,    //EMERGENCY_KEY  31

	EMERGENCY_KEY1=0x20,     //EMERGENCY_KEY  32

	HL_POWER0=0X21,			 //HL_POWER  33

	HL_POWER1=0X22,			 //HL_POWER  34


	//DISPLAY__KEY=0x1F,


	MAX_EVENTS /*keep it last */
}event_t;



typedef struct event_info
{
	event_t event;
}event_info_t;

typedef states_t (*action_t)(uint8_t* , uint8_t*, event_t);
typedef states_t  (*entry_action_t)(uint8_t* , uint8_t**, event_t);
typedef states_t (*exit_action_t)(uint8_t* , uint8_t**, event_t );

#define UNIFY_NULL         ((void *)0)

typedef struct state
{
	uint32_t id;
	uint8_t* info;
	action_t action;
	entry_action_t entry_action;
	exit_action_t exit_action;
}state_t;


typedef struct state_machine
{
	int32_t previous_state;
	uint32_t current_state;
	uint32_t fd;
}sm_t;

typedef struct st_info
{
	uint32_t idx;
	uint32_t previous_state;
	uint8_t row_idx;
	uint8_t col_idx;
}st_t;

/*uint8_t key;

uint8_t QueueCount = 0;
Queue DMR_Queue;
unsigned char queue_msg1[2]={'\0'},event='\0';
unsigned char queue_msg[2]={'\0'};*/

extern state_t* add_state_info(uint32_t id,entry_action_t entry_func,action_t action,exit_action_t exit_func, uint8_t* state_info);
extern uint32_t state_machine_run(uint8_t* sm_info, state_t* state, event_t event );

extern uint8_t  _idu_mmi_init(void);
extern int _idu_mmi_start(void);
void del_state_info(uint32_t id);

#endif /* __SM_H__ */
