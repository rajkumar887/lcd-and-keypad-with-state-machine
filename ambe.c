
#include "stdio.h"
#include "string.h"
#include "ambe.h"
#include "usart.h"
#include "radio.h"
#include "ADF4351.h"

extern uint8_t RxChar,RxChar1;
uint8_t ambe_count = 0;


AMBEType AMBE;	//structure holding 2 ambe packets, plus a flag to indicate if either is valid.

void Error(ErrorType error, uint16_t suberror)
{

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		HAL_UART_Receive_IT(&huart3,&RxChar,1);
		__HAL_UART_CLEAR_OREFLAG(&huart3);
		AMBE_Packet_FSM(RxChar);
		//HAL_UART_Transmit(&huart1,&RxChar,1,100);
	}
	if(huart->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1,&RxChar1,1);
		__HAL_UART_CLEAR_OREFLAG(&huart1);
		Set_Commands(RxChar1);
	}
}

typedef enum
{
	WAITING_FOR_START = 0,
	WAITING_FOR_LENGTH_MSB,
	WAITING_FOR_LENGTH_LSB,
	WAITING_FOR_TYPE,
	PAYLOAD,
} AMBE_Packet_FSM_Type;

void AMBE_Packet_FSM(uint8_t byte)
{
	static AMBE_Packet_FSM_Type state = WAITING_FOR_START;
	static uint16_t  i = 0,index=0;
	if(ambe_count == 1)
	{
		state = WAITING_FOR_START;
		ambe_count = 0;
		i = 0,index=0;
	}

	switch (state)
	{
	case WAITING_FOR_START:
		if (byte == 0x61)
			state = WAITING_FOR_LENGTH_MSB;
		break;

	case WAITING_FOR_LENGTH_MSB:
		AMBE.rx_packet[i].length = byte << 8;
		state = WAITING_FOR_LENGTH_LSB;
		break;
	case WAITING_FOR_LENGTH_LSB:
		AMBE.rx_packet[i].length |= byte;
		state = WAITING_FOR_TYPE;
		break;

	case WAITING_FOR_TYPE:
		AMBE.rx_packet[i].type = byte;
		state = PAYLOAD;
		//DEBUG
		if (AMBE.rx_packet[i].type == AMBE_PKT_CHAN)
			AMBE.packets_received++;

		break;

	case PAYLOAD:
		AMBE.rx_packet[i].data[index++] = byte;

		if (index == AMBE.rx_packet[i].length)
		{
			//complete packet rxd; we want to tell the foreground.
			//if forground hasn't read last packet, flag error.
			if (AMBE.current != NO_PACKET)
			{
				Error(AMBE_ERROR,0);
			}

			AMBE.current = i;	//signal to foreground
			i ^= 1;				//flip ping-pong buffer
			index = 0;
			state = WAITING_FOR_START;
			//AMBE.packets_received++;
		}
		break;

	default:
		break;
	}
}

#if 1
/*
 * Pass in pointer to buffer
 * wait for ISR to flag 'done' from last packet.
 * copy payload to ISR buffer (linear, packet)
 * kick off tx by sending 4-byte header (0x61, length, length, type)
 */

void AMBE_Send(uint8_t* payload, uint16_t length, uint8_t type)
{
	uint16_t i;

	//while(AMBE.tx_active);	//wait for ISR to finish sending last packet

	AMBE.tx_packet.length = length;
	AMBE.tx_packet.type = type;

	for (i=0 ; i<length ; i++)					//copy payload into ISR buffer
		AMBE.tx_packet.data[i] = payload[i];

	AMBE_SendPacket();
}

void AMBE_SendVoice(void)
{
#if 1
	uint16_t i;


	//while(AMBE.tx_active);	//wait for ISR to finish sending last packet

	AMBE.tx_packet.length = 38;				//36 data bytes (72 soft bits) + 2 control (CHAND4, #bits)
	AMBE.tx_packet.type = AMBE_PKT_CHAN;	//Channel packet
	AMBE.tx_packet.data[0] = AMBE_CHAND4;	//Channel data (4 bit soft metrics)
	AMBE.tx_packet.data[1] = 72;			//72 bits

	for (i=2 ; i<38 ; i++) //copy payload into ISR buffer
		AMBE.tx_packet.data[i] = VF.Rx[VF.AMBEIdx++];

	if (VF.AMBEIdx == 216)
		VF.AMBEIdx = 0;

	AMBE_SendPacket();
#else
	uint16_t i = 0;
	uint8_t start=0x61, msb=0, lsb=0;

	//while(AMBE.tx_active);	//wait for ISR to finish sending last packet

	AMBE.tx_packet.length = 38;				//36 data bytes (72 soft bits) + 2 control (CHAND4, #bits)
	AMBE.tx_packet.data[i++] = start;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length >> 8;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length;
	AMBE.tx_packet.data[i++] = AMBE_PKT_CHAN;
	AMBE.tx_packet.data[i++] = AMBE_CHAND4;
	AMBE.tx_packet.data[i++] = 72;

	for (i=6 ; i<42 ; i++) //copy payload into ISR buffer
		AMBE.tx_packet.data[i] = VF.Rx[VF.AMBEIdx++];

	HAL_UART_Transmit_IT(&huart3,&AMBE.tx_packet.data,(AMBE.tx_packet.length+4));

	if (VF.AMBEIdx == 216)
		VF.AMBEIdx = 0;
#endif
}
#else
void AMBE_Send(uint8_t* payload, uint16_t length, uint8_t type)
{
	uint16_t i = 0;
	uint8_t start=0x61, msb=0, lsb=0;

	while(AMBE.tx_active);	//wait for ISR to finish sending last packet

	AMBE.tx_packet.length = length;
	AMBE.tx_packet.type = type;

	/*for (i=0 ; i<length ; i++)					//copy payload into ISR buffer
		AMBE.tx_packet.data[i] = payload[i];*/

	AMBE.tx_packet.data[i++] = start;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length >> 8;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length;
	AMBE.tx_packet.data[i++] = type;

	for (i=4; i<(length+4) ; i++)					//copy payload into ISR buffer
		AMBE.tx_packet.data[i] = payload[i-4];

	HAL_UART_Transmit_IT(&huart3,&AMBE.tx_packet.data,(AMBE.tx_packet.length+4));


	//AMBE_SendPacket();
}

void AMBE_SendVoice(void)
{
	uint16_t i = 0;
	uint8_t start=0x61, msb=0, lsb=0;

	//while(AMBE.tx_active);	//wait for ISR to finish sending last packet

	AMBE.tx_packet.length = 38;				//36 data bytes (72 soft bits) + 2 control (CHAND4, #bits)
	AMBE.tx_packet.data[i++] = start;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length >> 8;
	AMBE.tx_packet.data[i++] = AMBE.tx_packet.length;
	AMBE.tx_packet.data[i++] = AMBE_PKT_CHAN;
	AMBE.tx_packet.data[i++] = AMBE_CHAND4;
	AMBE.tx_packet.data[i++] = 72;

	for (i=6 ; i<42 ; i++) //copy payload into ISR buffer
		AMBE.tx_packet.data[i] = VF.Rx[VF.AMBEIdx++];

	HAL_UART_Transmit_IT(&huart3,&AMBE.tx_packet.data,(AMBE.tx_packet.length+4));

	if (VF.AMBEIdx == 216)
		VF.AMBEIdx = 0;

	//AMBE_SendPacket();
}
#endif

void AMBE_SendPacket(void)
{
	uint8_t start=0x61, msb=0, lsb=0, type=0;
	AMBE.tx_count = 0;
	//AMBE.tx_active = 1;
	AMBE.tx_active = 0;

	msb = AMBE.tx_packet.length >> 8;
	lsb = AMBE.tx_packet.length & 0xff;
	type = AMBE.tx_packet.type;

	/*HAL_UART_Transmit(&huart3,&start,1,100);
	HAL_UART_Transmit(&huart3,&msb,1,100);
	HAL_UART_Transmit(&huart3,&lsb,1,100);
	HAL_UART_Transmit(&huart3,&type,1,100);
	HAL_UART_Transmit(&huart3,&AMBE.tx_packet.data,AMBE.tx_packet.length,100);*/

	HAL_UART_Transmit(&huart3,&start,1,10);
	HAL_UART_Transmit(&huart3,&msb,1,10);
	HAL_UART_Transmit(&huart3,&lsb,1,10);
	HAL_UART_Transmit(&huart3,&type,1,10);
	HAL_UART_Transmit(&huart3,&AMBE.tx_packet.data,AMBE.tx_packet.length,10);
	return;
}

int AMBE_Reset(void)
{
	uint32_t count;

	AMBE.current = NO_PACKET;
	AMBE.packets_sent = 0;
	AMBE.packets_received = 0;

	//AMBE_Soft_Reset();

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);	//AMBE Reset high
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);	//AMBE Reset low
	HAL_Delay(10);
	/*count = ReadCounter() + 10000;
	while (ReadCounter() < count);*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);	//AMBE Reset high
	//while (AMBE.current == NO_PACKET);
	HAL_Delay(10);
	count = ReadCounter() + 10000;
	while (AMBE.current == NO_PACKET)	//wait for packet
	{
		if (ReadCounter() > count)
		{
			//HAL_UART_Transmit(&huart1, "AMBE ERROR\n\r", 12, 10);
			printf("AMBE ERROR\n\r");
			Error(AMBE_ERROR,1);	//Error
			return 1;
		}
	}

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good; do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_READY	)
	{}
	else
	{
		Error(AMBE_ERROR,1);											//Error
		return 1;
	}

	AMBE.current = NO_PACKET;
	return 0;
}

void AMBE_Version(void)
{
	uint8_t ambe_payload[2];

	ambe_payload[0] = AMBE_PKT_VERSTRING;			//request version

	AMBE_Send(ambe_payload, 1, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_VERSTRING	)
	{
		strncpy(AMBE.version, (char*)&AMBE.rx_packet[AMBE.current].data[1],48);	//store reply
	}
	else
		Error(AMBE_ERROR,2);											//Error

	AMBE.current = NO_PACKET;
	return;
}

void AMBE_Set_Rate(uint16_t rate)
{
	uint8_t ambe_payload[2];

	ambe_payload[0] = AMBE_PKT_RATET;			//set rate
	ambe_payload[1] = rate;						//table entry 33 is 2450 speech + 1150 FEC = 3600 total (DMR standard)

	AMBE_Send(ambe_payload, 2, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good, do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_RATET	)
	{}
	else
		Error(AMBE_ERROR,3);											//Error

	AMBE.current = NO_PACKET;
	return;
}

void AMBE_Set_ECMode(uint16_t ecmode)
{
	uint8_t ambe_payload[3];

	ambe_payload[0] = AMBE_PKT_ECMODE;			//set encode flags
	ambe_payload[1] = ecmode >> 8;
	ambe_payload[2] = ecmode & 0xff;

	AMBE_Send(ambe_payload, 2, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good, do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_ECMODE	)
	{}
	else
		Error(AMBE_ERROR,4);											//Error

	AMBE.current = NO_PACKET;

	// DJL now initialise the decode mode as well

	ambe_payload[0] = AMBE_PKT_DCMODE;			//set decode flags to zero
	ambe_payload[1] = 0;
	ambe_payload[2] = 0;

	AMBE_Send(ambe_payload, 2, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good, do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_DCMODE	)
	{}
	else
		Error(AMBE_ERROR,4);											//Error

	AMBE.current = NO_PACKET;
	return;
}

void AMBE_Start_Codec(void)
{
	uint8_t ambe_payload[2];

	ambe_payload[0] = AMBE_PKT_CODECSTART;		//set rate
	ambe_payload[1] = AMBE_CODECSTART_SPI_PTD;	//SPI, passthrough disabled

	AMBE_Send(ambe_payload, 2, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good, do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_CODECSTART	)
	{}
	else
		Error(AMBE_ERROR,5);											//Error

	AMBE.current = NO_PACKET;
	return;
}


void AMBE_Stop_Codec(void)
{
	uint8_t ambe_payload[1];

	ambe_payload[0] = AMBE_PKT_CODECSTOP;		//set rate

	AMBE_Send(ambe_payload, 1, AMBE_PKT_CTRL);

	while (AMBE.current == NO_PACKET);	//wait for packet

	if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CTRL &&		//Packet is good, do nothing
			AMBE.rx_packet[AMBE.current].data[0] == AMBE_PKT_CODECSTOP	)
	{}
	else
		Error(AMBE_ERROR,5);											//Error

	AMBE.current = NO_PACKET;
	return;
}


void AMBE_Soft_Reset(void)
{
	uint8_t ambe_payload[1];

	ambe_payload[0] = AMBE_PKT_RESET;		//set rate

	AMBE_Send(ambe_payload, 1, AMBE_PKT_CTRL);

	return;
}
