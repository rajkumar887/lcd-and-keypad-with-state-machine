/**
 ******************************************************************************
 * @file		keypad4x4-scanning.c
 ******************************************************************************
 */


/** Includes ---------------------------------------------------------------- */
#include <keypad5x4-scanning.h>
#include"gpio.h"
GPIO_InitTypeDef _GPIO_InitStructKeypad;

int flag,flag1,flag2,flag3,flag4,flag5,flag6,flag7, flag8, flag9,dflag,Menu_flag,Menu_flag1,
Menu_flag2,Menu_flagrx,key_flag,key_flag1,key_flag2,key_flag3,key_flag4,key_flag5,key_flag6,
key_flag7,key_flag8;

int Num_Value=0;

void unify_KeypadInit()
{

	//DelayInit();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// GPIO Ports Clock Enable
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();


	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, COL1_Pin|COL2_Pin|COL3_Pin|COL4_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PEPin PEPin PEPin PEPin
	                           PEPin */
	GPIO_InitStruct.Pin = ROW1_Pin|ROW2_Pin|ROW3_Pin|ROW4_Pin
			|ROW5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PEPin PEPin PEPin PEPin */
	GPIO_InitStruct.Pin = COL1_Pin|COL2_Pin|COL3_Pin|COL4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);


#if 0
	DelayInit();

	// GPIO clock for keypad columns and rows
	RCC_APB2PeriphClockCmd(KEYPAD_RCC_GPIO_COL, ENABLE);
	RCC_APB2PeriphClockCmd(KEYPAD_RCC_GPIO_ROW, ENABLE);

	// Configure GPIO as output open drain for keypad columns
	_GPIO_InitStructKeypad.GPIO_Pin = KEYPAD_PIN_COL0 | KEYPAD_PIN_COL1 |
			KEYPAD_PIN_COL2 | KEYPAD_PIN_COL3;
	_GPIO_InitStructKeypad.GPIO_Mode = GPIO_Mode_Out_OD;
	_GPIO_InitStructKeypad.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(KEYPAD_GPIO_COL, &_GPIO_InitStructKeypad);

	// Configure GPIO as input with pull-up resistor for keypad rows
	_GPIO_InitStructKeypad.GPIO_Pin = KEYPAD_PIN_ROW0 | KEYPAD_PIN_ROW1 |
			KEYPAD_PIN_ROW2 | KEYPAD_PIN_ROW3;
	_GPIO_InitStructKeypad.GPIO_Mode = GPIO_Mode_IPU;
	_GPIO_InitStructKeypad.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(KEYPAD_GPIO_ROW, &_GPIO_InitStructKeypad);

#endif
}
/**
 ******************************************************************************
 * @brief	Get which key is pressed by scanning the columns and read the rows.
 * @param	None
 * @retval	Pressed key char value.
 ******************************************************************************
 */


uint8_t unify_KeypadGetKey()
{

HAL_GPIO_WritePin (KEYPAD_GPIO_COL,  KEYPAD_PIN_COL1, GPIO_PIN_SET);         //Pull the c0 high
HAL_GPIO_WritePin (KEYPAD_GPIO_COL,  KEYPAD_PIN_COL2, GPIO_PIN_RESET);     // Pull the c1 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL,  KEYPAD_PIN_COL3, GPIO_PIN_RESET);    // Pull the c2 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL,  KEYPAD_PIN_COL4, GPIO_PIN_RESET);    // Pull the c3 low

if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
{
	//tempvalue=testindex;
if(key_flag==0)
{
		Num_Value=0;key_flag=1;key_flag2=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag7=0;key_flag8=0;

	}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
			   key_flag=1;
			  Num_Value=Num_Value+1;            //1

		}
		   return'1';
		   break;
	  case 1:
		  	if(Num_Value==1 && key_flag==1)
			{
			   Num_Value=Num_Value+1;        //2
			  // tempvalue=tempvalue-1;       //-1
			   //testindex=tempvalue;        //-1
			 }
		  	return'.';
	  break;
	  case 2:
	  if(Num_Value==2 && key_flag==1)
			{
			   Num_Value=Num_Value+1;      //3
			  // tempvalue=tempvalue-1;      //-2
			   //testindex=tempvalue;        //-2
			}
		  return'-';
	  break;
	  case 3:
	  if(Num_Value==3 && key_flag==1)
		  {
			  Num_Value=0;
			  key_flag=0;
		//  tempvalue=tempvalue-1;          //-3
		  //testindex=tempvalue;
		  }
	  return'@';
		  break;
	 default:
	  break;
}

}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
{
	//tempvalue=testindex;
	if(key_flag1==0)
	{
	  Num_Value=0;key_flag=0;key_flag2=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag7=0;key_flag8=0;
	}
	switch(Num_Value)
	{
	  case 0:
		if(Num_Value==0)
			{
				key_flag1=1;
				Num_Value=Num_Value+1;          //1
			}
		   return'2';
	 break;
	  case 1:
		 if(Num_Value==1 && key_flag1==1)
			{
			  Num_Value=Num_Value+1;        //2
			 // tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'A';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag1==1)
			{
			  Num_Value=Num_Value+1;
			//tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'B';
	  case 3:
		  if(Num_Value==3 && key_flag1==1)
			  {
				  key_flag1=0;
			  Num_Value=0;
				//  tempvalue=tempvalue-1;
				 // testindex=tempvalue;
			  }
		  return'C';
		  break;
	 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
{
	//tempvalue=testindex;
	//tempvalue2=testindex;
	if(key_flag2==0)
	{
	  Num_Value=0;key_flag=0;key_flag1=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag7=0;key_flag8=0;
	}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag2=1;
				Num_Value=Num_Value+1;
			}
	   return'3';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag2==1)
			{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			  //testindex=tempvalue;
			}
		  return'D';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag2==1)
			{
			  Num_Value=Num_Value+1;
			 // tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'E';
	  case 3:
		 if(Num_Value==3 && key_flag2==1)
			{
			   Num_Value=Num_Value+1;
			//   tempvalue=tempvalue-1;
			 //  testindex=tempvalue;
			}
		  return'F';
		  break;
	 case 4:
		 if(Num_Value==4 && key_flag2==1)
			{
			  key_flag2=0;
			  Num_Value=0;
			//  tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'4';
		  break;
 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
{
	return '0';
}
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL1, GPIO_PIN_RESET);  ////Pull the c0 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL2, GPIO_PIN_SET);  //  Pull the c1 high
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL3, GPIO_PIN_RESET);  //  Pull the c2 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL4, GPIO_PIN_RESET);  //  Pull the c3 low
if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
{
	//tempvalue=testindex;
	if(key_flag3==0)
	{
		Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag7=0;key_flag8=0;
	}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag3=1;
				Num_Value=Num_Value+1;
			}
		   return'4';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag3==1)
			{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'G';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag3==1)
			{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'H';
	  case 3:
		 if(Num_Value==3 && key_flag3==1)
			{
			  key_flag3=0;
			  Num_Value=0;
			 // tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'I';
		  break;
	  default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
{
	//tempvalue=testindex;
	if(key_flag4==0)
		{
		   Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag3=0;key_flag5=0;key_flag6=0;key_flag7=0;key_flag8=0;
	}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag4=1;
				Num_Value=Num_Value+1;

			}
		   return'5';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag4==1)
			{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'J';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag4==1)
			{
			  Num_Value=Num_Value+1;
			 // tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'K';
	  case 3:
		 if(Num_Value==3 && key_flag4==1)
			{
			   key_flag4=0;
			   Num_Value=0;
			  // tempvalue=tempvalue-1;
			  // testindex=tempvalue;
			}
		  return'L';
		  break;
	  default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
{
	//tempvalue=testindex;
	if(key_flag5==0)
		{
		  Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag3=0;key_flag4=0;key_flag6=0;key_flag7=0;key_flag8=0;
		}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag5=1;
				Num_Value=Num_Value+1;
			}
		   return'6';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag5==1)
			{
			  Num_Value=Num_Value+1;
			//  tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'M';
		  break;
	  case 2:
	 if(Num_Value==2 && key_flag5==1)
			{
			  Num_Value=Num_Value+1;
			//  tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'O';
	/* case 3:
		 if(Num_Value==3 && key_flag5==1)
			{
			   key_flag5=0;
			   Num_Value=0;
			  // tempvalue=tempvalue-1;
			 //  testindex=tempvalue;
			}
		  return'';
		  break;*/
	 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
{
	return '0';
}
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL1, GPIO_PIN_RESET);  // Pull the c0 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL2, GPIO_PIN_RESET);  //  Pull the c1 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL3, GPIO_PIN_SET);  //  Pull the c2 high
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL4, GPIO_PIN_RESET);  // Pull the c3 low
if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
{
	//tempvalue=testindex;
	if(key_flag6==0)
		{
		  Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag7=0;key_flag8=0;
		}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag6=1;
				Num_Value=Num_Value+1;
			}
		   return'7';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag6==1)
			{
			  Num_Value=Num_Value+1;
			 // tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'P';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag6==1)
			{
			  Num_Value=Num_Value+1;
			//  tempvalue=tempvalue-1;
			  //testindex=tempvalue;
			}
		  return'Q';
	  case 3:
		 if(Num_Value==3 && key_flag6==1)
			{
			   key_flag6=0;
			   Num_Value=0;
			  // tempvalue=tempvalue-1;
			  // testindex=tempvalue;
			}
		  return'R';
		  break;
	  case 4:
	 		 if(Num_Value==3 && key_flag6==1)
	 			{
	 			   key_flag6=0;
	 			   Num_Value=0;
	 			  // tempvalue=tempvalue-1;
	 			  // testindex=tempvalue;
	 			}
	 		  return'S';
	 		  break;
	 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
{
	//tempvalue=testindex;
	if(key_flag7==0)
	{
		Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag8=0;
	}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag7=1;
				Num_Value=Num_Value+1;
			}
		   return'8';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag7==1)
			{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'T';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag7==1)
		{
			  Num_Value=Num_Value+1;
			  //tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'U';
	  case 3:
		 if(Num_Value==3 && key_flag7==1)
			{
			  key_flag7=0;
			  Num_Value=0;
			 // tempvalue=tempvalue-1;
			  //testindex=tempvalue;
			}
		  return'V';
		  break;
	 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
{
	//tempvalue=testindex;
	if(key_flag8==0)
		{
			Num_Value=0;key_flag=0;key_flag1=0;key_flag2=0;key_flag3=0;key_flag4=0;key_flag5=0;key_flag6=0;key_flag7=0;
		}
	switch(Num_Value)
	{
	  case 0:
			if(Num_Value==0)
			{
				key_flag8=1;
				Num_Value=Num_Value+1;
			}
		   return'9';
		 break;
	  case 1:
		 if(Num_Value==1 && key_flag8==1)
			{
			   Num_Value=Num_Value+1;
			  // tempvalue=tempvalue-1;
			  // testindex=tempvalue;
			}
		  return'W';
		  break;
	  case 2:
		 if(Num_Value==2 && key_flag8==1)
			{
		  Num_Value=Num_Value+1;
			 // tempvalue=tempvalue-1;
			//  testindex=tempvalue;
			}
		  return'X';
	  case 3:
		 if(Num_Value==3 && key_flag8==1)
			{
			  Num_Value=Num_Value+1;
			 // tempvalue=tempvalue-1;
			 // testindex=tempvalue;
			}
		  return'Y';
		  break;
	 case 4:
		 if(Num_Value==4 && key_flag8==1)
			{
			  key_flag8=0;
			  Num_Value=0;
			  //tempvalue=tempvalue-1;
			  //testindex=tempvalue;
			}
		  return'Z';
		  break;
	 default:
		  break;
	}
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
{
	return '#';
}
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL1, GPIO_PIN_RESET);  // Pull the c0 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL2, GPIO_PIN_RESET);  //  Pull the c1 low
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL3, GPIO_PIN_RESET);  //  Pull the c2 LOW
HAL_GPIO_WritePin (KEYPAD_GPIO_COL, KEYPAD_PIN_COL4, GPIO_PIN_SET);  // Pull the c3 HIGH
if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
{
	return 'a';
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
{
	return 'b';
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
{
	return 'c';
}
else if(HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
{
	return 'd';
}
else
	//return 0;
	return KEYPAD_NO_PRESSED;
}




#if 0

uint8_t unify_KeypadGetKey()
{
	// Scan column 1 (column 1 pin is grounded, other column pins is open drain)
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL1,0);   //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL2,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL3,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL4,1);


	HAL_Delay(5);
	//DelayUs(1);
	// Read rows
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
		return '1';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
		return '5';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
		return '9';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
		return 'B';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW5))
		return 'U';

	// Scan column 2 (column 2 pin is grounded, other column pins is open drain)
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL1,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL2,0);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL3,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL4,1);

	HAL_Delay(5);
	//DelayUs(1);
	// Read rows
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
		return '2';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
		return '6';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
		return '*';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
		//return 'M';
		return 'E';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW5))
		return 'L';

	// Scan column 3 (column 3 pin is grounded, other column pins is open drain)
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL1,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL2,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL3,0);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL4,1);

	HAL_Delay(5);
	//DelayUs(1);
	// Read rows
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
		return '3';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
		return '7';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
		return '0';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
		return 'C';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW5))
		return 'E';

	// Scan column 4 (column 4 pin is grounded, other column pins is open drain)
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL1,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL2,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL3,1);
	HAL_GPIO_WritePin(KEYPAD_GPIO_COL, KEYPAD_PIN_COL4,0);

	HAL_Delay(5);
	//DelayUs(1);
	// Read rows
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW1))
		return '4';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW2))
		return '8';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW3))
		return '#';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW4))
		return 'D';
	if (! HAL_GPIO_ReadPin(KEYPAD_GPIO_ROW, KEYPAD_PIN_ROW5))
		return 'R';

	return KEYPAD_NO_PRESSED;
}
#endif
/********************************* END OF FILE ********************************/
/******************************************************************************/
