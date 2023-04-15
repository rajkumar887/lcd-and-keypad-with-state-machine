/*
 * dmr.c
 *
 *  Created on: Oct 25, 2022
 *      Author: dmr1
 */

#include <cmx7341.h>
#include <spi_bitbang.h>
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "ambe.h"
#include "usart.h"
#include "dmr.h"
#include "radio.h"

void DMRTx(void);
void TxVoiceHeader(void);
void TxVoiceFrame(void);
void TxDataHeader(void);
void TxDataPacket(void);
void TxCSBK(void);

#define VOICE_SYNC 0xA300
#define VOICE_EMB  0xAD00

extern UART_HandleTypeDef huart3;
extern uint8_t RxChar;
extern uint8_t ambe_count;
uint8_t Rx_Active = 0;

CMX7341_Type cmx7341;
uint16_t sync;

static bool call_connection = false;

//1st word burst type / burst info / errors ; 2nd byte slot count ; 3rd byte colour code / data type
// then 9 bytes of payload. 24-bit CRC added by modem in Tx, not reported in Rx (see error field)
uint16_t VoiceHeader[12] = {0x8400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x00FC, 0x00DF, 0x00FF, 0x00FF, 0x00FF};
uint16_t TERM_LC[12]     = {0x9400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00FF, 0x00FC, 0x00DF, 0x00FF, 0x00FF, 0x00FF};
uint16_t EMB_LC_0[12]    = {0x7000, 0x0000, 0x0010, 0x0000, 0x0000, 0x0008, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0038};

//1st word burst type / burst info / errors ; 2nd byte slot count ; 3rd byte colour code / PI / LCSS
// then 2 bytes of payload.
uint16_t EMB_RC_0[5]     = {0xEE00, 0x0000, 0x0010, 0x0007, 0x0089};

//1st word burst type / burst info / errors ; 2nd byte slot count ; 3rd byte colour code / data type
// then 10 bytes of payload. 16-bit CRC added by modem in Tx, not reported in Rx (see error field)
uint16_t CSBK[13]          = {0xB400, 0x0000, 0x0013, 0x00BD, 0x0000, 0x00C0, 0x0014, 0x0000, 0x0000, 0x0001, 0x0000, 0x0000, 0x0001};
uint16_t DataHeader[15]    = {0xB400, 0x0000, 0x0016, 0x008D, 0x00A0, 0x0000, 0x0000, 0x0001, 0x0000, 0x0000, 0x0001, 0x0059, 0x0050, 0x0000, 0x0000};
uint16_t DataHalfRate[15]  = {0xB400, 0x0000, 0x0017, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

//2 header words + 3*9-byte frames. Data packed for Tx.
uint16_t AMBE_1K[29]     = {0xA300,0x0000,0x00CE,0x00A8,0x00FE,0x0083,0x00AC,0x00C4,0x0058,0x0020,0x000A,0x00CE,0x00A8,0x00FE,0x0083,0x00AC,0x00C4,0x0058,0x0020,0x000A,0x00CE,0x00A8,0x00FE,0x0083,0x00AC,0x00C4,0x0058,0x0020,0x000A};

uint16_t LcControlDataRead[8];

const char Msg1[] = "DE9945 Data Message using rate 1/2 data";
const char Msg2[] = "Hello World from the DE9945";

void StartDMRTx(void);
void DMRRx(void);
void ReadDataPacket(uint16_t TypeInfo);
void AgcConfig(void);
//Payload sizes for data blocks. Use Data Type as index
//                         PI  V.Hdr V.Trm CSBK MBC MBC Data 1/2 3/4 Idle 1   Reserved...
uint8_t PayloadSize[16] = {10, 9,    9,    10,  0,  0,  12,  12, 18, 12,  24, 0, 0, 0, 0, 0};



void start_DMR_Rx(void)
{
	call_connection = false;
	Spi_Write_HalfWord(POWERDOWN_CTRL,0xB9C4); 	//power up CMX7341 features for RXIQ

	//Input gain now handled by Modem AGC
	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE, 0x02D8);	//Disc->IP1, ALT->IP2 ; IP1 Gain = IP2 Gain = 0dB
	//OP1->Audio, Bias->MOD1, Bias->MOD2

	SetAOGDMR();	//Set gains for mod outputs and audio volume

	AgcConfig();

	Spi_Write_HalfWord(MODEM_CONFIG,0x03FF);	//detect all syncs

	Spi_Write_HalfWord(MODE_CONTROL,0x0003);	//Tx/Rx - (re)start sync search

	while (SPI_Read_HalfWord(RXFIFO_LEVEL))		//flush Rx FIFO
		SPI_Read_HalfWord(RXFIFO_WORD);

	Spi_Write_HalfWord(SPI_CODEC_CTRL, 0);	//stop SPI Codec
	Radio.CodecOn = 0;

	Radio.RadioState = DMR_RX_IDLE;
	Radio.Header = 0;
	Radio.RxCount = 0;

	VF.AMBEIdx = 0;
	VF.CMX7341Idx = 108;
	//HAL_UART_Transmit(&huart1,"start DMR Rx\n\r", 14, 10);
	printf("start DMR Rx\n\r");
}

void DMR_Rx_Idle_original(void)
{
	uint16_t ambe_reset = 1;
	//if (cmx7341.IRQn)
	{
		cmx7341.IRQn = 0;
		//cmx7341.Status |= SPI_Read_HalfWord(STATUS);	//Read the status reg


		cmx7341.Status = 0;
		cmx7341.Status = SPI_Read_HalfWord(STATUS);
		cmx7341.Status = cmx7341.Status & FRAMESYNC;
		while((cmx7341.Status != FRAMESYNC)&& (Radio.RadioState == DMR_RX_IDLE))
		{
			cmx7341.Status = SPI_Read_HalfWord(STATUS);
			cmx7341.Status = cmx7341.Status & FRAMESYNC;
		}

		cmx7341.Status &= ~FRAMESYNC;	//clear flag
		sync = SPI_Read_HalfWord(FS_STATUS) >> 12;

		//HAL_UART_Transmit(&huart1, "DMR Rx idle", sizeof(MSG), 10);

		ambe_reset = AMBE_Reset();
		if (ambe_reset==0)	//reset ambe
		{
			AMBE_Version();
			AMBE_Set_Rate(33);                //table entry 33 is 2450 speech + 1150 FEC = 3600bps total (DMR standard)
			AMBE_Set_ECMode(0x0040);	      //enable noise supression
			AMBE_Start_Codec();
		}
		Radio.RadioState = DMR_RX_ACTIVE;
	}
}


void DMR_Rx_Idle(void)
{
	uint16_t ambe_reset = 1;
	//if (cmx7341.IRQn)
	{
		cmx7341.IRQn = 0;
		cmx7341.Status = 0;
		cmx7341.Status = SPI_Read_HalfWord(STATUS);
		cmx7341.Status = cmx7341.Status & FRAMESYNC;
		if(cmx7341.Status != FRAMESYNC)
		{
			cmx7341.Status = SPI_Read_HalfWord(STATUS);
			cmx7341.Status = cmx7341.Status & FRAMESYNC;
		}
		if(cmx7341.Status == FRAMESYNC  && Radio.RadioState == DMR_RX_IDLE)
		{
			cmx7341.Status &= ~FRAMESYNC;	//clear flag
			sync = SPI_Read_HalfWord(FS_STATUS) >> 12;
			HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_SET);
			HAL_UART_Transmit(&huart1, "FS\n\r", 4, 10);
			if(ambe_count==1 || Rx_Active)
			{
				Rx_Active = 0;
				HAL_NVIC_EnableIRQ(USART3_IRQn);
				HAL_UART_Receive_IT(&huart3,&RxChar,1);
			}
			ambe_reset = AMBE_Reset();
			if (ambe_reset==0)	//reset ambe
			{
				AMBE_Version();
				AMBE_Set_Rate(33);                //table entry 33 is 2450 speech + 1150 FEC = 3600bps total (DMR standard)
				AMBE_Set_ECMode(0x0040);	      //enable noise supression
				AMBE_Start_Codec();
			}
			Radio.RadioState = DMR_RX_ACTIVE;
		}


	}
}

void DMR_Rx_Active(void)
{
	Rx_Active = 1;
	//if (cmx7341.IRQn)
	{
		cmx7341.IRQn = 0;
		cmx7341.Status |= SPI_Read_HalfWord(STATUS);	//Read the status reg

		if (cmx7341.Status & RX_READY)		//If the CMX7341 has a new frame
		{
			//HAL_UART_Transmit(&huart1, "status\n\r", 8, 10);
			//HAL_UART_Transmit(&huart1, &cmx7341.Status, 2, 10);
			cmx7341.Status &= ~RX_READY;	//clear flag
			//HAL_UART_Transmit(&huart1, "clear Status\n\r", 14, 10);
			//HAL_UART_Transmit(&huart1, &cmx7341.Status, 2, 10);
			DMRRx();
		}
	}

	//if (VF.CMX7341Idx > 36)//(AMBE.current != NO_PACKET)	//We have a new AMBE packet
	{
		//HAL_UART_Transmit(&huart1,&VF.CMX7341Idx , 1, 10);
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		if(call_connection)
			AMBE_SendVoice();			//ignore data from AMBE, but send Rx'd data
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		AMBE.current = NO_PACKET;
	}

}
//retreive DMR packet from modem, process
void DMRRx(void)
{
	uint16_t i,level,temp,type; //temp1;

	level = SPI_Read_HalfWord(RXFIFO_LEVEL);

	if (level > 127)	//110 for voice packet + 12 for EMB LC + 1 for CACH +4 for CACH SLC
	{
		//Error(RX_PACKET_ERROR,0);
		//HAL_UART_Transmit(&huart1, "RX_PACKET_ERROR\n\r", 16, 10);
		printf("RX_PACKET_ERROR\n\r");
		Spi_Write_HalfWord(SPI_CODEC_CTRL, 0);	//stop SPI Codec
		HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_RESET);
		//AMBE_Stop_Codec();
		ambe_count=1;
		Radio.CodecOn = 0;
		Radio.RadioState = START_DMR_RX;
		return;
	}

	while (level != 0)
	{
		temp = SPI_Read_HalfWord(RXFIFO_WORD);			//read first word

		type = temp>>12;

		switch(type)
		{
		case CACH_TACT:
			break;
		case CACH_SLC:
			for (i=0 ; i<4 ; i++)
				Spi_Read_byte_cmx7341(RXFIFO_BYTE);		//read and discard CACH SLC data.
			break;

		case LC_EMB:
			LCEMBPacket.BurstType = type;
			LCEMBPacket.BurstInfo = temp >> 8 & 0x000f;
			LCEMBPacket.Errors = temp & 0xff;

			LCEMBPacket.Timing = Spi_Read_byte_cmx7341(RXFIFO_BYTE);

			for (i=0 ; i<10 ; i++)
				LCEMBPacket.Data[i] = Spi_Read_byte_cmx7341(RXFIFO_BYTE);

			break;

		case LC_VOICE_HDR:
			ReadDataPacket(temp);
			break;

		case LC_VOICE_TERM:
			ReadDataPacket(temp);
			Spi_Write_HalfWord(SPI_CODEC_CTRL, 0);	//stop SPI Codec
			//HAL_UART_Transmit(&huart1, "DMR Rx done\n\r", 13, 10);
			HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_RESET);
			printf("DMR Rx done\n\r");
			ambe_count=1;
			Radio.CodecOn = 0;
			Radio.RadioState = START_DMR_RX;
			break;

		case VOICE_PAYLOAD:
			//if(call_connection)
			{
				VoicePacket.BurstType = type;
				VoicePacket.BurstInfo = temp >> 8 & 0x000f;
				VoicePacket.Errors = temp & 0xff;

				VoicePacket.Timing = Spi_Read_byte_cmx7341(RXFIFO_BYTE);

				HAL_NVIC_DisableIRQ(USART3_IRQn);

				for (i=0 ; i<108 ; i++)
				{
					VF.Rx[VF.CMX7341Idx++] =  Spi_Read_byte_cmx7341(RXFIFO_BYTE);
				}
				if (VF.CMX7341Idx == 216)
					VF.CMX7341Idx = 0;

				HAL_NVIC_EnableIRQ(USART3_IRQn);
				//Count packets
				if (VoicePacket.BurstInfo == 1)			//BS Voice Sync
					Radio.RxCount = 0;					//reset count
				else if (VoicePacket.BurstInfo == 3)	//MS Voice Sync
					Radio.RxCount = 0;					//reset count
				else if (VoicePacket.BurstInfo == 6)	//MS Voice Sync TDMA slot1
					Radio.RxCount = 0;					//reset count
				else if (VoicePacket.BurstInfo == 8)	//MS Voice Sync TDMA slot2
					Radio.RxCount = 0;					//reset count
				else
					Radio.RxCount++;				//inc count

				if (Radio.RxCount == 1)			//now rx'd 2 voice packets; start codec
				{
					if (Radio.RxMode == RXMODE_PASSTHROUGH || Radio.RxMode == RXMODE_EYE )
					{}
					else
					{
						if (!Radio.CodecOn)
						{
							Spi_Write_HalfWord(SPI_CODEC_CTRL, 1);	//start SPI Codec
							Radio.CodecOn = 1;
						}
					}
				}

				if (Radio.RxCount >= VOICE_TIMEOUT)			//too many packets without sync; terminate.
				{
					//HAL_UART_Transmit(&huart1, "Voice time out\n\r", 16, 10);
					printf("Voice time out\n\r");
					HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_RESET);
					ambe_count = 1;
					Radio.RadioState = START_DMR_RX;
					Spi_Write_HalfWord(SPI_CODEC_CTRL, 0);	//stop SPI Codec
					Radio.CodecOn = 0;
				}
			}

			break;

		case DATA:
			ReadDataPacket(temp);
			switch (DataPacket.DataType)
			{
			case DT_CSBK:

				break;
			case DT_DATA_HDR:
				Radio.Msgptr = Radio.Message;
				Radio.Packets = DataPacket.Data[1] & 0xff;	//Appended Blocks field
				Radio.Frame = 0;
				break;
			case DT_RATE_12:

				i=1;	//sepura

				for ( ; i<12 ; i+=2)					//read data from packet into message buffer
					*Radio.Msgptr++=DataPacket.Data[i];

				Radio.Frame++;							//count frames

				if (Radio.Frame == Radio.Packets)			//if it's the last frame
				{
					Radio.Msgptr = Radio.Message;	//find end of string
					while (*Radio.Msgptr != 0)
						Radio.Msgptr++;

					Radio.EndOfMessage = Radio.Msgptr;	//remember end (used for wrapping display)
					Radio.Msgptr = Radio.Message;		//pointer to start

					Radio.RadioState = DMR_RX_DISPLAY_MESSAGE;
				}

				break;

			}

			break;

			case EMB_DATA:
				RCEMBPacket.BurstType = type;
				RCEMBPacket.BurstInfo = temp >> 8 & 0x000f;
				RCEMBPacket.Errors = temp & 0xff;

				RCEMBPacket.Timing = Spi_Read_byte_cmx7341(RXFIFO_BYTE);

				for (i=0 ; i<3 ; i++)
					RCEMBPacket.Data[i] = Spi_Read_byte_cmx7341(RXFIFO_BYTE);
				break;

			default:
				Error(RX_PACKET_ERROR,1);
				return;
				break;
		}
		level = SPI_Read_HalfWord(RXFIFO_LEVEL);
	}
}

void ReadDataPacket(uint16_t TypeInfo)
{
	uint16_t temp,i;
	uint32_t SID,DID;

	DataPacket.BurstType = TypeInfo >> 12;
	DataPacket.BurstInfo = TypeInfo >> 8 & 0x000f;
	DataPacket.Errors = TypeInfo & 0xff;

	DataPacket.Timing = Spi_Read_byte_cmx7341(RXFIFO_BYTE);

	temp = Spi_Read_byte_cmx7341(RXFIFO_BYTE);
	DataPacket.ColourCode = temp>>4;
	DataPacket.DataType = temp & 0x0f;

	for (i=0 ; i<PayloadSize[DataPacket.DataType] ; i++)
	{
		DataPacket.Data[i] = Spi_Read_byte_cmx7341(RXFIFO_BYTE);
		printf("D:%x\n\r",DataPacket.Data[i]);
	}

	SID = ((DataPacket.Data[3]<<16) & 0x00FF0000) | ((DataPacket.Data[4]<<8) & 0x0000FF00) | (DataPacket.Data[5] & 0x0000FF);
	DID = ((DataPacket.Data[6]<<16) & 0x00FF0000) | ((DataPacket.Data[7]<<8) & 0x0000FF00) | (DataPacket.Data[8] & 0x0000FF);

	printf("Source ID:%ld\n\r",SID);
	printf("Destination ID:%ld\n\r",DID);

	if(SID == 0xFFFFFF)
	{
		call_connection = true;
		printf("Broadcast call established\n\r");
	}
	if(SID == 0xFFFCDF)
	{
		call_connection = true;
		printf("Group call established\n\r");
	}
	if(SID == 0x5)
	{
		call_connection = true;
		printf("Individual call established\n\r");
	}
	return;
}

void StartDMRTx(void)
{
	uint16_t i;//,ipgainroute;
	uint16_t words=0,Ambe_reset = 1;
	Radio.Msgptr = NULL;

	Spi_Write_HalfWord(POWERDOWN_CTRL,0xFFC4);
	Spi_Write_HalfWord(MODE_CONTROL,0x0030);
	Radio.Slot = (SPI_Read_HalfWord(SLOT_NUMBER) + 5) & 0xff;	//set start slot
	//HAL_UART_Transmit(&huart1, "DMR Tx start\n\r", 14, 10);
	printf("DMR Tx start\n\r");
	Radio.TxMode =  TXMODE_VOICE;
	HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_SET);
	switch(Radio.TxMode)
	{
	case TXMODE_VOICE:
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		HAL_UART_Receive_IT(&huart3,&RxChar,1);
		ambe_count=1;
		Ambe_reset = AMBE_Reset();
		//HAL_Delay(1000);
		if (Ambe_reset == 0)
		{
			AMBE_Version();
			AMBE_Set_Rate(33); //table entry 33 is 2450 speech + 1150 FEC = 3600bps total (DMR standard)
			AMBE_Set_ECMode(0x0040);	//enable noise supression
			AMBE_Start_Codec();
		}
		VF.AMBEIdx = 0;
		VF.CMX7341Idx = 27;

		TxVoiceHeader();								//first header to kick it off
		Radio.Header = 2;									//2 more before we start voice frames
		break;

	case TXMODE_AMBE_TEST_TONE:

		for (i=0 ; i<27 ; i++)							//init voice frame with 1k tone.
			VF.Tx[i] = AMBE_1K[i+2];

		TxVoiceHeader();								//first header to kick it off
		Radio.Header = 2;									//2 more before we start voice frames
		break;

	case TXMODE_MSG1:
		Radio.Msgptr = (char*)Msg1;			//each byte takes 1 word to tx
		//fall through
	case TXMODE_MSG2:
		if (Radio.Msgptr == NULL)
			Radio.Msgptr = (char*)Msg2;
		words = strlen(Radio.Msgptr) + 1;	//each byte takes 1 word to tx; +1 for null terminator.
		Radio.Packets = (words + 3)/6;	//2 words for CRC, 1 word at start - otherwise, 6 words per packet
		if (((words+3) % 6) != 0)		//if there's a remainder
			Radio.Packets++;				//send one more packet.

		Radio.Frame = Radio.Packets + 1 + 16;	//1 header, 16 CSBKs
		TxCSBK();								//first one to kick it off

		break;

	case TXMODE_MOD_SETUP:
		Spi_Write_HalfWord(SETUP_PATTERN,0x55FF);
		Spi_Write_HalfWord(MODE_CONTROL,0x00A1);	//start modem into tx mod mode.
		break;

	case TXMODE_PRBS:
		Spi_Write_HalfWord(MODE_CONTROL,0x0091);	//start modem into tx prbs mode.
		break;

	}

}

void ServiceCMX7341Tx()
{
	//if (cmx7341.IRQn)						//If we've had an interrupt
	{
		cmx7341.Status |= SPI_Read_HalfWord(STATUS);	//Read the status reg

		cmx7341.IRQn = 0;

		if (cmx7341.Status & TX_READY)		//If the CMX7341 is ready for another packet...
		{
			cmx7341.Status &= ~TX_READY;	//clear flag
			DMRTx();						//...give it one.
		}
	}
	return;
}

void ServiceAMBETx(void)
{
	int i;

	if (AMBE.current != NO_PACKET)	//We have a new AMBE packet
	{
		if (AMBE.rx_packet[AMBE.current].type == AMBE_PKT_CHAN)
		{
			if (AMBE.rx_packet[AMBE.current].length == 11)	//2 header bytes + 9 data
			{
				if (AMBE.rx_packet[AMBE.current].data[0] == AMBE_CHAND)
				{
					if (AMBE.rx_packet[AMBE.current].data[1] == 72)	//9 data bytes = 72 bits
					{
						for (i=2 ; i<11 ; i++)
						{
							//HAL_UART_Transmit(&huart1,&AMBE.rx_packet[AMBE.current].data[i],1,100);
							VF.Tx[VF.AMBEIdx++] = AMBE.rx_packet[AMBE.current].data[i];
						}
						if (VF.AMBEIdx == 27)
							VF.CMX7341Idx = 0;

						if (VF.AMBEIdx == 54)
						{
							VF.AMBEIdx = 0;
							VF.CMX7341Idx = 27;
						}
						AMBE.current = NO_PACKET;
						return;	//to avoid error message
					}
				}
			}
		}
		AMBE.current = NO_PACKET;
		//Error(AMBE_ERROR, 6);	//AMBE packet error
	}
	return;
}

void DMRTx(void)
{
	Radio.TxMode =  TXMODE_VOICE;
	switch(Radio.TxMode)
	{
	case TXMODE_VOICE:
		if (Radio.Header)
		{
			TxVoiceHeader(); //Tx header
			Radio.Header--;
			if (Radio.Header == 1)
				Spi_Write_HalfWord(SPI_CODEC_CTRL, 0x12 | (cmx7341.Param.MicGain << 10));	//start SPI Codec in Tx; set gain, set hpf
		}
		else
		{
			TxVoiceFrame();			//Tx frame
		}

		break;

	case TXMODE_MSG1:
	case TXMODE_MSG2:
		if (Radio.Frame > Radio.Packets+1)			//Start sending CSBKs
			TxCSBK();
		else if (Radio.Frame == Radio.Packets+1)		//After 16 CSBKS send 1 header
			TxDataHeader();
		else if (Radio.Frame == 0)				//finished so back to RX, but wait fro Tx to finish.
		{
			Radio.RadioState = START_DMR_RX;
			while(SPI_Read_HalfWord(SLOT_NUMBER)<= Radio.Slot);
		}
		else									//else, send data packet.
			TxDataPacket();
		break;

	case TXMODE_AMBE_TEST_TONE:
		if (Radio.Header)
		{
			TxVoiceHeader(); 		//Tx header
			Radio.Header--;
		}
		else
		{
			VF.CMX7341Idx = 0;
			TxVoiceFrame();			//Tx frame
		}

		break;

	case TXMODE_MOD_SETUP:

		break;

	case TXMODE_PRBS:

		break;
	}
}

void TxVoiceHeader(void)
{
	uint16_t i;

	VoiceHeader[1] = Radio.Slot;				//set slot number for transmission

	for (i=0 ; i<12 ; i++)					//load data
		Spi_Write_HalfWord(TXFIFO_WORD,VoiceHeader[i]);

	Radio.Slot = (Radio.Slot + 2) & 0xff;		//next slot.
}

void TxDataHeader(void)
{
	uint16_t i;

	DataHeader[1] = Radio.Slot;				//set slot number for transmission
	DataHeader[4] = Radio.Packets & 0x000F;	//packet count

	for (i=0 ; i<15 ; i++)					//load data
		Spi_Write_HalfWord(TXFIFO_WORD,DataHeader[i]);

	Radio.Slot = (Radio.Slot + 2) & 0xff;		//next slot.
	Radio.Frame--;
}

void TxDataPacket(void)
{
	uint16_t i;

	DataHalfRate[1] = Radio.Slot;				//set slot number for transmission

	for (i=4 ; i<15 ; i+=2)					//SEPURA; fill packet //character every 2 bytes
		DataHalfRate[i] = *Radio.Msgptr++;

	for (i=0 ; i<15 ; i++)					//load data
		Spi_Write_HalfWord(TXFIFO_WORD,DataHalfRate[i]);

	Radio.Slot = (Radio.Slot + 2) & 0xff;		//next slot.
	Radio.Frame--;
}

void TxCSBK(void)
{
	uint16_t i;

	CSBK[1] = Radio.Slot;				//set slot number for transmission
	CSBK[6] = Radio.Frame;			//countdown

	for (i=0 ; i<13 ; i++)					//load data
		Spi_Write_HalfWord(TXFIFO_WORD,CSBK[i]);

	Radio.Slot = (Radio.Slot + 2) & 0xff;		//next slot.
	Radio.Frame--;
}

void TxVoiceFrame(void)
{
	uint16_t i;

	if (Radio.Frame == 0)			//Start of superframe
	{
		if (Radio.Terminate)		//if ptt released send a terminator
		{
			TERM_LC[1] = Radio.Slot;				//put slot in packet with sync
			for (i=0 ; i<12 ; i++)					//load data
				Spi_Write_HalfWord(TXFIFO_WORD,TERM_LC[i]);

			Spi_Write_HalfWord(SPI_CODEC_CTRL, 0);	//stop SPI Codec
			//HAL_UART_Transmit(&huart1, "DMR Tx done\n\r", 13, 10);
			printf("DMR Tx done\n\r");
			HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
			ambe_count=1;
			Radio.Terminate = 0;
			Radio.RadioState = START_DMR_RX;

			while(SPI_Read_HalfWord(SLOT_NUMBER)<= Radio.Slot);
		}
		else //not end of call, send voice + sync + embedded in first frame
		{
			EMB_LC_0[1] = Radio.Slot;					//put slot in LC
			for (i=0 ; i<12 ; i++)					//load data
				Spi_Write_HalfWord(TXFIFO_WORD,EMB_LC_0[i]);

			EMB_RC_0[1] = Radio.Slot;					//put slot in RC
			for (i=0 ; i<5 ; i++)					//load data
				Spi_Write_HalfWord(TXFIFO_WORD,EMB_RC_0[i]);

			Spi_Write_HalfWord(TXFIFO_WORD,VOICE_SYNC);
			Spi_Write_HalfWord(TXFIFO_WORD,Radio.Slot);

			//NVIC_DisableIRQ(USART0_IRQn);
			HAL_NVIC_DisableIRQ(USART3_IRQn);
			if (VF.CMX7341Idx == 27)
			{}
			else if(VF.CMX7341Idx == 0)
			{}
			else
			{
				//Error(AMBE_ERROR,8);
			}
			for (i=0 ; i<27 ; i++)					//load data
				Spi_Write_HalfWord(TXFIFO_WORD,VF.Tx[VF.CMX7341Idx++]);

			//NVIC_EnableIRQ(USART0_IRQn);
			HAL_NVIC_EnableIRQ(USART3_IRQn);
			Radio.Frame = 5;	//send 5 more voice frames before next sync
		}
	}
	else // not start of superframe, send voice + embedded
	{
		Spi_Write_HalfWord(TXFIFO_WORD,VOICE_EMB);
		Spi_Write_HalfWord(TXFIFO_WORD,Radio.Slot);

		//NVIC_DisableIRQ(USART0_IRQn);
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		if (VF.CMX7341Idx == 27)
		{}
		else if(VF.CMX7341Idx == 0)
		{}
		else
		{
			//Error(AMBE_ERROR,8);
		}
		for (i=0 ; i<27 ; i++)					//load data
			Spi_Write_HalfWord(TXFIFO_WORD,VF.Tx[VF.CMX7341Idx++]);

		//NVIC_EnableIRQ(USART0_IRQn);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		Radio.Frame--;
	}

	Radio.Slot = (Radio.Slot + 2) & 0xff;		//always update slot.
}

/***********AGC***********/
void AgcConfig(void)
{
	Spi_Write_HalfWord(POWERDOWN_CTRL,0xFFC4);	//power up everything
	Spi_Write_HalfWord(AUX_ADC_CTRL,0x0000); 	//Reset ADC
	Spi_Write_HalfWord(AUX_ADC_CTRL,0x1A3C);	 //set AuxADC1 input to AUX1, set averaging mode 1A19
	Spi_Write_HalfWord(AUX_ADC1_THRESH_HI,0x0285); //set high threshold = $185
	Spi_Write_HalfWord(AUX_ADC1_THRESH_LO,0x0280);  //set low threshold = $180

	Spi_Write_HalfWord(AUX_ADC2_THRESH_HI,0x0285); //set high threshold = $185
	Spi_Write_HalfWord(AUX_ADC2_THRESH_LO,0x0280);  //set low threshold = $180
	return;
}
