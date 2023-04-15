#include <keypad5x4-scanning.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Queue.h"
#include "sm.h"
#include "main.h"
#include "rotary.h"
#include "lcd.h"
#include "Radio.h"
#include "Analogue.h"

uint8_t key;
extern uint8_t key_rotary;
extern uint8_t key_PTT;
extern uint8_t key_squelch;
uint8_t key_emergency;
extern uint8_t key_HL_power;
extern bool DMR_Mode;
extern uint16_t squelch_flag, squelch_gpio_flag;
uint8_t prv_key_rotary = 255, prev_PTT_rev_key =27, prev_PTT_tran_key =3;
uint8_t key_disp;
uint8_t prev_squelch_rev_key =30,prev_emergency_rev_key=32,prev_hl_power_rev_key=34;
//uint8_t

uint8_t PTT_Flag = 1;
Queue DMR_Queue;
uint8_t QueueCount = 0;
unsigned char queue_msg1[2]={'\0'},event='\0';
unsigned char queue_msg[2]={'\0'};

#define KEYPAD_EVENT  1
#define ROTARY_EVENT  2
#define PTT_EVENT     3
#define SQUELCH_EVENT 4
#define HL_POWER_EVENT 5
#define EMERGENCY_EVENT 6

struct state_list
{
	struct state_list  *next;
	state_t* state;	
};

struct state_list head; 
struct state_list *state_gp = &head; 

/*-------------------------------------------------------------------
/// Adding the state to list of states
--------------------------------------------------------------------*/
void add_to_list(state_t* state)
{
	struct state_list  *tmp = state_gp;
	for(;tmp->next != NULL; tmp=tmp->next);
	tmp->next = ( struct state_list *) calloc(1, sizeof( struct state_list));
	tmp->next->state = state;
	tmp->next->next = NULL;
}

/*-------------------------------------------------------------------
/// Delete state from list of states
--------------------------------------------------------------------*/
state_t* del_from_list(uint8_t id)
{
	struct state_list  *tmp = state_gp, *prev;

	if(id > MAX_STATES) 
	{
		return 0;
	}

	prev = tmp->next;
	tmp = tmp->next;
	for(;tmp != NULL; tmp=tmp->next)
	{
		if(tmp->state->id == id)
		{
			prev->next = tmp->next;
			return tmp->state;
		}
		prev = tmp;
	}
	if (tmp == NULL)
	{
		printf("\nState :%d not found to delete\n", id);
	}
	return 0;
}

/*-------------------------------------------------------------------
/// Getting the state details to state machine
--------------------------------------------------------------------*/
state_t* get_state_info(uint8_t id)
{
	struct state_list  *tmp = state_gp;
	//int i=0;
	tmp = tmp->next;
	for(;tmp != NULL; tmp=tmp->next)
	{
		//printf("%d-state:0x%x\n\r",i,tmp->state->id);
		//i=i+1;
		if(tmp->state->id == id)
		{	
			return tmp->state;
		}
	}
	return NULL;
}

/*-------------------------------------------------------------------
/// Adding the state information to state machine
--------------------------------------------------------------------*/
state_t* add_state_info(uint32_t id, entry_action_t entry_func, action_t action, exit_action_t exit_func, uint8_t* state_info)
{
	state_t*  state = NULL;
	if (entry_func == NULL || action == NULL ||  exit_func == NULL)
	{
		printf("Adding new state %d failed .. \n", id);
		return state;
	}
	/* state = (state_t*)us_wlan_alloc_memory(sizeof(state_t)); */
	state = (state_t*)malloc(sizeof(state_t));
	printf("state added to list\n\r");
	if (state == NULL)
	{
		printf("Memory allocation failed \n");
		return state;
	}
	state->id = id; 
	state->action = action;
	state->entry_action = entry_func;
	state->exit_action = exit_func;
	state->info = state_info;

	/* Add it to list */
	add_to_list(state);
	return state;
}

/*-------------------------------------------------------------------
/// deleting the state information from state machine
--------------------------------------------------------------------*/
void del_state_info(uint32_t id)
{
	state_t*  state;
	state = del_from_list(id);
	printf("state is %x\n\r",(unsigned int *)state);
}

/*-------------------------------------------------------------------
/// Executing the state machine
--------------------------------------------------------------------*/
uint32_t state_machine_run(uint8_t* sm_info, state_t* state, event_t event) /* These args are default values */
{
	//Queue _queue;
	int32_t ret = 0;
	state_t *current_state;
	unsigned char queue_msg[YY_BUF_SIZE]; //,queue_msg1[YY_BUF_SIZE];
	//unsigned char queue_msg3[YY_BUF_SIZE],queue_msg4[YY_BUF_SIZE];
	event_t evnt ;
	//sm_t* info = (sm_t*)sm_info;
	uint8_t mmi_sm_should_run=1;
	current_state = state;

	/* Return the current state action */ 
	ret = current_state->entry_action(sm_info, &current_state->info, event);
	if (ret == -1)
	{
		printf("Entry function failed \n\r");
		goto out;	
	}	

	while (mmi_sm_should_run)
	{

		key_rotary= unify_RotaryGetKey();
		key= unify_KeypadGetKey();

		//	key = KeyPad_WaitForKeyGetChar(1);

		key_PTT = unify_PTT_switch();

		//key_squelch=unify_SQUELCH_switch();

		//	key_HL_power=unify_HL_POWER_switch();

		/*key_emergency=unify_EMERGENCY_switch();

			key_disp= unify_display_switch();*/


		if( key!=NULL)
		{
			queue_msg[0] = KEYPAD_EVENT;
			queue_msg[1] =  key;
			enqueue(&DMR_Queue,queue_msg);
		}

		if(key_rotary!=NULL)
		{
			if(prv_key_rotary != key_rotary)
			{
				HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_SET);
				rotary_information(key_rotary);
				HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
				queue_msg[0] = ROTARY_EVENT;
				queue_msg[1] = key_rotary;
				prv_key_rotary = key_rotary;
				enqueue(&DMR_Queue,queue_msg);
			}
		}


		if(prev_PTT_rev_key != key_PTT)
		{
			queue_msg[0] = PTT_EVENT;
			queue_msg[1] = key_PTT;
			prev_PTT_rev_key = key_PTT;
			enqueue(&DMR_Queue,queue_msg);
		}

		/* Squelch functionality in FM mode */
		if (squelch_flag && squelch_gpio_flag && !DMR_Mode)
		{
			Unify_Squelch();
			squelch_flag=0;
		}
		/*
			if(prev_squelch_rev_key != key_squelch)
			{
				queue_msg[0] = SQUELCH_EVENT;
				queue_msg[1] = key_squelch;
				prev_squelch_rev_key = key_squelch;
				enqueue(&DMR_Queue,queue_msg);
			}

			if(prev_emergency_rev_key != key_emergency)
			{
				queue_msg[0] = EMERGENCY_EVENT;
				queue_msg[1] = key_emergency;
				prev_emergency_rev_key = key_emergency;
				enqueue(&DMR_Queue,queue_msg);
			}
			if(prev_hl_power_rev_key != key_HL_power)
			{
				queue_msg[0] = HL_POWER_EVENT;
				queue_msg[1] = key_HL_power;
				prev_hl_power_rev_key = key_HL_power;
				enqueue(&DMR_Queue,queue_msg);
			}
		 */


		DoRadio();

		if(getQueueCount(&DMR_Queue)>0)
		{
			unsigned int i=0;
			dequeue(&DMR_Queue,queue_msg1);
			//dequeue(&DMR_Queue,queue_msg1);
			printf("dq msg: ");
			for(i=0; queue_msg1[i]!='\0' ; i++)
				printf("0x%x ",queue_msg1[i]);
			printf("\n\r");
			switch(queue_msg1[0])
			{

			case ROTARY_EVENT:
			{

				// if ROTARY interrupt is generated
				evnt = (event_t)queue_msg1[1] ;
				break;
			}

			case KEYPAD_EVENT:
			{

				/* if keypad interrupt is generated */
				evnt = (event_t)queue_msg1[1] ;
				break;
			}

			case  PTT_EVENT:
			{
				//if PTT interrupt is generated
				evnt = (event_t)queue_msg1[1] ;
				break;
			}

			case SQUELCH_EVENT:
			{
				//if squelch interrupt is generated
				evnt = (event_t)queue_msg1[1] ;
				break;
			}

			case HL_POWER_EVENT:
			{
				evnt = (event_t)queue_msg1[1] ;
				break;
			}

			case EMERGENCY_EVENT:
			{
				evnt = (event_t)queue_msg1[1] ;
				break;
			}
			}

			if (evnt == UNIFY_NULL)
			{

				continue;
			}





			/*--------------------------------------------------------
					//return the current state action of the state machine 
					//param1 - state machine information
					//param2 - current state information
					//param3 - event
					----------------------------------------------------------*/
			ret = current_state->action(sm_info, current_state->info, evnt);  /* use previous state here */
			if (ret == -1)
			{
				printf("Action function failed \n\r");
				goto out;
			}
			else
			{
				//printf("state returns %d \n\r",ret);
				/* getting the state information from state machine */
				state =  get_state_info(ret);
				if (state == NULL)
				{
					printf("State information not found \n\r");
				}
				/*printf("Next state id %d \n", state->id); */
			}

			if (state && state != current_state)
			{
				/*--------------------------------------------------------
						//returning current state exit or action
						//param1 - state machine information
						//param2 - current state information
						//param3 - event
						----------------------------------------------------------*/

				ret = current_state->exit_action(sm_info,&current_state->info,(event_t)event);
				if (ret == -1)
				{
					printf("Exit function failed \n\r");
					goto out;
				}
				current_state = state;
				/*--------------------------------------------------------
						//returning current state exit or action
						//param1 - state machine information
						//param2 - current state information
						//param3 - event
						----------------------------------------------------------*/
				ret = current_state->entry_action(sm_info, &current_state->info, (event_t)event);
				if (ret == -1)
				{
					printf("Entry function failed \n\r");
					goto out;
				}
				evnt=(event_t)NULL;
			}

		}

	}
	out:
	return ret;
}


