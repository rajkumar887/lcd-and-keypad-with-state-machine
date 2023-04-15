/************************************************************************************************************
 *
 *  $Copyright � 2015-2016 Applied Technology UK $
 *
 *  $Header: radio.c  Revision:1.79  25 August 2016 11:41:53  ilewis $
 *  $Label: 06-Sep-16 EF9945-2.0.0.1 $
 *
 *  THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 *  WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 *  TIME. AS A RESULT, CML MICROSYSYEMS SHALL NOT BE HELD LIABLE FOR ANY
 *  DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 *  FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 *  CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 *************************************************************************************************************/
#include <cmx7341.h>
#include <spi_bitbang.h>
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#include "main.h"
#include "radio.h"
#include "ambe.h"
#include "dmr.h"
#include "usart.h"
#include "Analogue.h"
#include "rotary.h"
#include "ADF4351.h"


extern struct Channel_parameters Channel_pointer;
extern uint8_t Rx_Active;
RadioType Radio;
VoiceFrameType VF;

RCEMBPacketType RCEMBPacket;
LCEMBPacketType LCEMBPacket;
DataPacketType DataPacket;
VoicePacketType VoicePacket;

uint16_t check_status = 0, freq_cntrl = 0x1D8,count=0;

ChannelType Channel_Table[MAX_CHANNEL_TABLE+1] =
{
		{"D01 ", 0xC1, 0x55},                  //Dxx= DMR
		{"D02 ", 0xC4, 0x00},
		{"D03 ", 0xC6, 0xAB},
		{"D04 ", 0xC9, 0x55},
		{"D05 ", 0xCC, 0x00},
		{"D06 ", 0xCE, 0xAB},
		{"D07 ", 0xD1, 0x55},
		{"D08 ", 0xD4, 0x00},
		{"dP01", 0xC0, 0xAB},                 //Dpxx = dPMR
		{"dP02", 0xC2, 0x00},
		{"dP03", 0xC3, 0x55},
		{"dP04", 0xC4, 0xAB},
		{"dP05", 0xC6, 0x00},
		{"dP06", 0xC7, 0x55},
		{"dP07", 0xC8, 0xAB},
		{"dP08", 0xCA, 0x00},
		{"dP09", 0xCB, 0x55},
		{"dP10", 0xCC, 0xAB},
		{"dP11", 0xCE, 0x00},
		{"dP12", 0xCF, 0x55},
		{"dP13", 0xD0, 0xAB},
		{"dP14", 0xD2, 0x00},
		{"dP15", 0xD3, 0x55},
		{"dP16", 0xD4, 0xAB},
		{"A01 ", 0xAC, 0x00},                   //Axx = Analogue
		{"A02 ", 0xAE, 0xAB},
		{"A03 ", 0xB1, 0x55},
		{"A04 ", 0xB4, 0x00},
		{"A05 ", 0xB6, 0xAB},
		{"A06 ", 0xB9, 0x55},
		{"A07 ", 0xBC, 0x00},
		{"A08 ", 0xBE, 0xAB}
};


void CheckChannel(void);
void CheckVolumeDMR(void);
void CheckVolumeAnalogue(void);
void ConfigCMX994(void);
void LoadRampProfile(void);
void ConfigTxSequence(void);
void ConfigTxSequence_dPMR(void);
void ProgWrite (uint16_t addr, uint16_t data);
void NextProgWrite(uint16_t data);
void ProgBlock6 (void);
void AgcConfig(void);
void Analogue_ProgBlock0(void);
void Analogue_ProgBlock2(void);
void StartAnalogueTx(void);
void AudioPAon (void);
void AudioPAoff (void);
void TxAttn (void);
void InitFI2DMRRx(void);
void InitFI2DMRTx(void);
void InitFI2DMR(void);

uint16_t mod1fine,mod1coarse,mod2fine,mod2coarse;     //global variables

void DoRadio(void)
{
	double calcfreq;
	//Radio.RadioState =  START_DMR_TX;
	switch (Radio.RadioState)
	{
	case RADIO_IDLE:
		break;

	case START_DMR_RX:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			UpdateFrequencyRegisters(Channel_pointer.rxfrequency, 38400000.0, 1000, 1, 1, &calcfreq);
			PLL_Sync();
			PLL_LD_Check(1000);
		}
		InitFI2DMRRx();
		start_DMR_Rx();
		break;

	case DMR_RX_IDLE:
		DMR_Rx_Idle();
		break;

	case DMR_RX_ACTIVE:
		DMR_Rx_Active();
		break;

	case START_DMR_TX:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			UpdateFrequencyRegisters(Channel_pointer.txfrequency, 38400000.0, 1000, 1, 1, &calcfreq);
			PLL_Sync();
			PLL_LD_Check(1000);
		}
		InitFI2DMRTx();
		StartDMRTx();
		Radio.RadioState = DMR_TX;
		break;

	case DMR_TX:
		ServiceCMX7341Tx();
		ServiceAMBETx();
		break;

	case START_ANALOGUE_RX:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			UpdateFrequencyRegisters(Channel_pointer.rxfrequency, 38400000.0, 1000, 1, 1, &calcfreq);
			PLL_Sync();
			PLL_LD_Check(1000);
		}
		Unify_Analogue_Rx();
		Radio.RadioState = ANALOGUE_RX_IDLE;
		break;

	case ANALOGUE_RX_IDLE:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			//HAL_Delay(1000);
			check_status = 0;
			if(!(check_status & 0x0008))
				check_status = SPI_Read_HalfWord(STATUS);

			if(check_status & 0x0008)
			{
				//printf("DTMF sync: %x\n\r",check_status);
				check_status = 0;
				check_status = SPI_Read_HalfWord(0xCC);
				check_status = check_status & 0xFF00;
			}

			if(check_status == 0x4000 || check_status == 0x4100 || check_status == 0x4200
					|| check_status == 0x4300 || check_status == 0x4400)
			{
				HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_SET);
				check_status = 0;
				//printf("DTMF read: %x\n\r",check_status);
				printf("Call initiated\n\r");
				Spi_Write_HalfWord(MODE_CONTROL,0x8201);
				Radio.RadioState = ANALOGUE_RX_ACTIVE;
			}
		}
		else
		{
			//HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_SET);
			Radio.RadioState = ANALOGUE_RX_ACTIVE;
		}
		break;

	case ANALOGUE_RX_ACTIVE:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			//Check_FM_Mode_Rx_Status();
			check_status = 0;
			if(!(check_status & 0x0008))
				check_status = SPI_Read_HalfWord(STATUS);

			if(check_status & 0x0008)
			{
				//printf("DTMF sync: %x\n\r",check_status);
				check_status = 0;
				check_status = SPI_Read_HalfWord(0xCC);
				check_status = check_status & 0xFF00;
			}

			if(check_status == 0x4500 || check_status == 0x4600 || check_status == 0x4700
					|| check_status == 0x4800 || check_status == 0x4900)
			{
				check_status = 0;
				//printf("DTMF read: %x\n\r",check_status);
				printf("Call terminated\n\r");
				HAL_GPIO_WritePin(G_LED_GPIO_Port,G_LED_Pin,GPIO_PIN_RESET);
				Spi_Write_HalfWord(MODE_CONTROL,0x8001);
				HAL_Delay(200);
				Radio.RadioState = START_ANALOGUE_RX;
			}
		}
		break;

	case START_ANALOGUE_TX:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			UpdateFrequencyRegisters(Channel_pointer.txfrequency, 38400000.0, 1000, 1, 1, &calcfreq);
			PLL_Sync();
			PLL_LD_Check(1000);
		}
		HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_SET);
		Unify_Analogue_Tx();
		Radio.RadioState = ANALOGUE_TX;
		break;

	case ANALOGUE_TX:
		if(Channel_pointer.channel_type == 2 || Channel_pointer.channel_type == 3)
		{
			if(Radio.Terminate == 1)
			{
				Radio.RadioState = START_ANALOGUE_RX;
				Radio.Terminate = 0;
				printf("Call terminated\n\r");
				HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
				Spi_Write_HalfWord(0xC1, 0x0202);
				count = 0;
				while(count<5)
				{
					Spi_Write_HalfWord(0xB6,(count+5));
					HAL_Delay(30);
					Spi_Write_HalfWord(0xB6, 0x0080);
					HAL_Delay(30);
					count++;
				}

				Spi_Write_HalfWord(0xC1, 0x8002);
			}
		}
		else
		{
			if(Radio.Terminate == 1)
			{
				Radio.RadioState = START_ANALOGUE_RX;
				Radio.Terminate = 0;
				printf("Call terminated\n\r");
				HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
				Spi_Write_HalfWord(0xC1, 0x8002);
			}
		}
		break;

	default:
		Radio.RadioState = START_DMR_RX;
		break;

	}

	return;
}

void TxAttn ( )
{
	/*if (MMI.MsgOut == KEY_A)	MMI.Mod1Attn++;
	else if (MMI.MsgOut == KEY_B)	MMI.Mod1Attn--;
	else if (MMI.MsgOut == KEY_C)	MMI.Mod2Attn++;
	else if (MMI.MsgOut == KEY_D)	MMI.Mod2Attn--;

	MMI.MsgOut = 0;

	if (MMI.Mod1Attn > 69) MMI.Mod1Attn = 69;		//70 steps, 0 to 69
	if (MMI.Mod1Attn <  0) MMI.Mod1Attn = 0;
	if (MMI.Mod2Attn > 69) MMI.Mod2Attn = 69;		//70 steps, 0 to 69
	if (MMI.Mod2Attn <  0) MMI.Mod2Attn = 0;

	mod1fine   = MMI.Mod1Attn % 10;
	mod1coarse = 7 - (MMI.Mod1Attn / 10);
	mod2fine   = MMI.Mod2Attn % 10;
	mod2coarse = 7 - (MMI.Mod2Attn / 10);*/
}

void AudioPAon (void)
{
	//Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 5, 2, 0);	   //Audio PA SHDN false
}

void AudioPAoff (void)
{
	//Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 5, 2, 1);	   //Audio PA SHDN true
}

void CheckChannel(void)
{
	/*if (MMI.MsgOut & (KEY_A | KEY_B) )
	{
		if (MMI.MsgOut == KEY_A) //Channel Up.
		{
			Radio.Channel++;
			if (Radio.Channel > MAX_CHANNEL_TABLE)
				Radio.Channel = 0;

			if (Radio.Channel == MIN_DMR_CHANNEL)
				GoToDMR();
			else if (Radio.Channel == MIN_dPMR_CHANNEL)
				GoTodPMR();
			else if (Radio.Channel == MIN_ANALOGUE_CHANNEL)
				GoToAnalogue();
		}
		else 					//Must be KEY_B: channel down
		{
			Radio.Channel--;
			if (Radio.Channel < 0)
				Radio.Channel = MAX_CHANNEL_TABLE;

			if (Radio.Channel == MAX_DMR_CHANNEL)
				GoToDMR();
			else if (Radio.Channel == MAX_dPMR_CHANNEL)
				GoTodPMR();
			else if (Radio.Channel == MAX_ANALOGUE_CHANNEL)
				GoToAnalogue();
		}

		Radio_SkyWorks(Channel_Table[Radio.Channel].frac1, Channel_Table[Radio.Channel].frac2);

		sprintf(MMI.DisplayBot,"CH:%s  Vol:%02d",Channel_Table[Radio.Channel].Name,cmx7341.AudioAttn);

		MMI.MsgIn[MMI.MsgInCount++] = UPDATE_LCD;
		MMI.MsgOut = 0;
	}*/
}

void CheckVolumeDMR(void)
{
	/*if (MMI.MsgOut & (KEY_C | KEY_D) )
	{
		if (MMI.MsgOut == KEY_C) //vol Up.
		{
			if (cmx7341.AudioAttn < MAX_VOL)
				cmx7341.AudioAttn++;
		}
		else 					//Must be KEY_D: vol down
		{
			if (cmx7341.AudioAttn > MIN_VOL)
				cmx7341.AudioAttn--;
		}

		SetAOGDMR();
		MMI.MsgOut = 0;
		sprintf(MMI.DisplayBot,"CH:%s  Vol:%02d",Channel_Table[Radio.Channel].Name,cmx7341.AudioAttn);
		MMI.MsgIn[MMI.MsgInCount++] = UPDATE_LCD;
	}*/
}

void CheckVolumeAnalogue(void)
{/*
	if (MMI.MsgOut & (KEY_C | KEY_D) )
	{
		if (MMI.MsgOut == KEY_C) //vol Up.
		{
			if (cmx7341.AudioAttn < MAX_VOL)
				cmx7341.AudioAttn++;
		}
		else 					//Must be KEY_D: vol down
		{
			if (cmx7341.AudioAttn > MIN_VOL)
				cmx7341.AudioAttn--;
		}

		SetAOGAnalogue();
		MMI.MsgOut = 0;
		sprintf(MMI.DisplayBot,"CH:%s  Vol:%02d",Channel_Table[Radio.Channel].Name,cmx7341.AudioAttn);
		MMI.MsgIn[MMI.MsgInCount++] = UPDATE_LCD;
	}*/
}


void StartAnalogueTx(void)
{
	Radio.Msgptr = NULL;

	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x12B4);        //Output1->MOD1,Output2->MOD2,MIC->Input1,DISC->Input2, Input 1 Gain - 12.8dB (Mic I/P ~100mVp-p) from script
	Spi_Write_HalfWord(MODEM_CONF_FI1,0x8000);           // enable the Tx sequencer

	SPI_Read_HalfWord(STATUS);					//flush status

	switch(Radio.TxMode)
	{
	case TXMODE_VOICE:

		Spi_Write_HalfWord(MODE_CONTROL,0x8002);         //TX Voice mode  (from script)
		break;

	case TXMODE_MOD_SETUP:
		Spi_Write_HalfWord(MODE_CONTROL,0x4002);         //TX 1k mode  (from script)
		break;

	case TXMODE_AMBE_TEST_TONE:
		//sprintf(MMI.DisplayTop,"Tx 1k         ");
		Spi_Write_HalfWord(MODE_CONTROL,0x4002);	//start modem into 1k tone mode
		break;

	}
}



void wait_status(void)
{
	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG)));
}

uint16_t RampProfile[64] =
{
		000,000,455,460,465,470,475,480,
		486,494,502,510,516,522,528,534,
		540,545,550,555,560,565,570,574,
		578,582,586,590,594,598,602,606,
		610,614,618,622,625,628,631,634,
		637,640,643,646,649,652,655,658,
		661,664,667,670,673,676,679,682,
		685,688,691,694,697,700,703,703,
};

void ProgWrite (uint16_t addr, uint16_t data)
{
	while((SPI_Read_HalfWord(IRQ_STATUS_REG) & PROGBIT) == 0);
	Spi_Write_HalfWord(PROG_ADDRESS, addr);
	Spi_Write_HalfWord(PROG_REG, data);
}

void NextProgWrite(uint16_t data)
{
	while((SPI_Read_HalfWord(IRQ_STATUS_REG) & PROGBIT) == 0);
	Spi_Write_HalfWord(PROG_REG, data);
}

void LoadRampProfile(void)
{
	uint16_t i;

	Spi_Write_HalfWord(MODE_CTRL,0x0000);

	while((SPI_Read_HalfWord(IRQ_STATUS_REG) & PROGBIT) == 0);
	Spi_Write_HalfWord(PROG_ADDRESS, 0x0700);

	for (i=0 ; i<64 ; i++)
		NextProgWrite(RampProfile[i]);
}

void ConfigTxSequence(void)
{
	Spi_Write_HalfWord(MODE_CTRL,0x0000);
	while((SPI_Read_HalfWord(IRQ_STATUS_REG) & PROGBIT) == 0);

	Spi_Write_HalfWord(PROG_ADDRESS, 0x0302);	//Start writing tx sequencer at P.3.2

	NextProgWrite(0x0006);	//TXENA switch-on time -      P.3.2 (default $0006)
	NextProgWrite(0x0007);	//RAMDAC ramp-up start time - P.3.3 (default $000C)
	NextProgWrite(0x0012);	//GPIOA switch-on time -      P.3.4 (default $0012)
	NextProgWrite(0x0018);	//GPIOB switch-on time -      P.3.5 (default $0018)
	NextProgWrite(0x0024);	//Modulation start time -     P.3.6 (default $0024)
	NextProgWrite(0x0003);	//Modulation vernier adjust - P.3.7 (default $0003)
	NextProgWrite(0x0342);	//RAMDAC ramp-up start time - P.3.8 (default $0342)
	NextProgWrite(0x034e);	//GPIOB switch-off time -     P.3.9 (default $034E)
	NextProgWrite(0x0354);	//GPIOA switch-off time -     P.3.10 (default $0354)
	NextProgWrite(0x035a);	//TXENA switch-off time -     P.3.11 (default $035A)
	NextProgWrite(0x035b);	//RXENA switch-on time -     P.3.12 (default $035A)

	//NextProgWrite(0x0618);	//RAMDAC Scan Rate - P.3.13 (default $0518 = 505uS)

	return;
}

void config_CMX994_Rx(void)
{
	Spi_Write_HalfWord(MODE_CTRL,0x0000); //prog through Idle mode

	check_status = 0;
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);
	while(PROG_READY != check_status)
		check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F11); //General Cntrl Reg, interrupt when accepted, but SPI incomplete
	Spi_Write_HalfWord(PROG_REG,0x00C0); //Disable PLL - 80h to use ext LO and PLL
	//;copy    #$90,*PROG_REG		;Disable PLL - 80h to use ext LO and PLL, Enable Low Power Mode
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F25); //VCO Control Reg
	Spi_Write_HalfWord(PROG_REG,0x0010);
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F12); //Rx Cntrl Reg
	Spi_Write_HalfWord(PROG_REG,0x10); //10 is div by 2 and max BW, 08 is div by 2 and mid BW
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F16); //Rx gain Reg - note if AGC on this write is ignored
	Spi_Write_HalfWord(PROG_REG,0x08); //00 = Max gain in 100R mode and 08 = max gain in 50R mode
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F14); //IM Reg
	Spi_Write_HalfWord(PROG_REG,0x3F);	//3F for best IM performance but slightly more gain at 00hex
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F13); //DC offset reg, only use this manual control if FI doesn't do auto calibration
	Spi_Write_HalfWord(PROG_REG,0xA0); //Default 00hex, A0 = 202593
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F15); //CMX994A and CMX994E Options reg
	Spi_Write_HalfWord(PROG_REG,0x00); //Default 00hex
	wait_status();
}

void config_CMX994_Tx(void)
{
	Spi_Write_HalfWord(MODE_CTRL,0x0000); //prog through Idle mode

	check_status = 0;
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);
	while(PROG_READY != check_status)
		check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F11); //General Cntrl Reg, interrupt when accepted, but SPI incomplete
	Spi_Write_HalfWord(PROG_REG,0x00C0); //Disable PLL - 80h to use ext LO and PLL
	//;copy    #$90,*PROG_REG		;Disable PLL - 80h to use ext LO and PLL, Enable Low Power Mode
	wait_status();

	Spi_Write_HalfWord(PROG_ADDRESS,0x0F25); //VCO Control Reg
	Spi_Write_HalfWord(PROG_REG,0x0010);
	wait_status();

}

void Test_Pattern_2pmod(void)
{
	//Spi_Write_HalfWord(TX_TEST_PATT,block); //set Tx Test Pattern, to setup two point modulation

	/* we configured below 2 registers are for getting 4FSK Tx signals with minimum  errors */
	Spi_Write_HalfWord(TX_OP_CNTRL, 0x6400); //Tx Coarse Gain Control, MOD1 = -2dB and MOD2 = -8dB for sn 202589
	Spi_Write_byte_cmx7341(TX_FINE_CNTRL,0x55); //Tx Fine Gain Control, MOD1 = -0.6dB and MOD2 = -0.6dB for sn 202589
	Spi_Write_HalfWord(RAMDAC_Level,0x02D5); //DEFAULT_RAMDACSCALING //Reduce Output Level to do Tx Mod Setup - max output at $03FF
	//Spi_Write_HalfWord(MODE_CTRL,0x0020); //Tx Test Mode Enabled, note Tx will Output Power <+20dBm

	Spi_Write_HalfWord(PROG_ADDRESS,0x0600);
	Spi_Write_HalfWord(PROG_REG,0x0000);

}


void GoToDMR(void)
{
	//cmx7341FI = (cmxFI_TypeDef*) START_FI_A;	//FI Area A has FI-2, for DMR
	//cmx7341.FI.activation_code1 = FI2ACT1;
	//cmx7341.FI.activation_code2 = FI2ACT2;

	cmx7341.Mode = MODE_DMR;

	//InitRadio();
	//DoMMI();	//allow reporting of error messages
	InitFI2DMR();
	return;
}

void GoToAnalogue(void)
{
	//cmx7341FI = (cmxFI_TypeDef*) START_FI_B;	//FI Area B has FI-1, for Analogue
	//cmx7341.FI.activation_code1 = FI1ACT1;
	//cmx7341.FI.activation_code2 = FI1ACT2;

	cmx7341.Mode = MODE_ANALOGUE;

	//InitRadio();
	//DoMMI();	//allow reporting of error messages
	InitFI1Analogue();
}

void NoModeChange(void){}

void SetAOGDMR(void)	//Set Analogue Output gains for DMR
{
	Spi_Write_HalfWord(ANALOG_OP_GAIN, 0xF); //cmx7341.Param.CoarseModAttnDMR | cmx7341.AudioAttn);
	Spi_Write_byte_cmx7341 (ANALOG_CTRL,0x0); //cmx7341.Param.FineModAttnDMR);			//fine gain
}

void SetAOGdPMR(void)	//Set Analogue Output gains for dPMR
{
	Spi_Write_HalfWord(ANALOG_OP_GAIN, cmx7341.Param.CoarseModAttndPMR | cmx7341.AudioAttn);
	Spi_Write_byte_cmx7341 (ANALOG_CTRL,cmx7341.Param.FineModAttndPMR);			//fine gain
}

void SetAOGAnalogue(void)	//Set Analogue Output gains for Analogue
{
	Spi_Write_HalfWord(ANALOG_OP_GAIN, cmx7341.Param.CoarseModAttnAnalogue | cmx7341.AudioAttn);
	Spi_Write_byte_cmx7341 (ANALOG_CTRL,cmx7341.Param.FineModAttnAnalogue);			//fine gain
}

void InitRadio(void)
{

	//DE9945_AMBE_UART_Init();//init UART 0, used by AMBE vocoder
	GoToDMR();
	/*if (AMBE_Reset()== 0)	//0 for success
	{
		AMBE_Version();
		AMBE_Set_Rate(33); 			//table entry 33 is 2450 speech + 1150 FEC = 3600bps total (DMR standard)
		AMBE_Set_ECMode(0x0040);	//enable noise supression
		AMBE_Start_Codec();
	}*/

	//copy boot values from flash structure to RAM
	/*cmx7341.FI.db1_ptr = cmx7341FI->db1_ptr;
	cmx7341.FI.db1_len = cmx7341FI->db1_len;
	cmx7341.FI.db1_start_address = cmx7341FI->db1_start_address;

	cmx7341.FI.db2_ptr = cmx7341FI->db2_ptr;
	cmx7341.FI.db2_len = cmx7341FI->db2_len;
	cmx7341.FI.db2_start_address = cmx7341FI->db2_start_address;

	cmx7341.FI.activate_len = cmx7341FI->activate_len;
	cmx7341.FI.activate_ptr  = cmx7341FI->activate_ptr;

	cmx7341.FI.db1_chk_hi = cmx7341FI->db1_chk_hi;
	cmx7341.FI.db1_chk_lo = cmx7341FI->db1_chk_lo;
	cmx7341.FI.db2_chk_hi = cmx7341FI->db2_chk_hi;
	cmx7341.FI.db2_chk_lo = cmx7341FI->db2_chk_lo;

	if (cmx7341.FI.db1_start_address < (uint16_t*)START_FI_A || cmx7341.FI.db1_start_address > (uint16_t*)START_RADIO_APP)
	{
		Error(FI_LOAD_ERROR,0);
		return;
	}

	if (SDR_Load_FI_7x4x(&cmx7341.FI, 0) == 0)
		Error(FI_LOAD_ERROR,1);*/

	return;
}

void InitFI2DMRRx (void)
{
	//uint16_t rx_fifo_byte = 0;

	Rx_Active = 1;
	if(Rx_Active)
	{
		Rx_Active = 0;
		InitFI2DMRTx();
		StartDMRTx();
		while(Radio.Frame<5)

		{
			ServiceCMX7341Tx();
			ServiceAMBETx();

			while(Radio.Frame < 5)
			{
				Radio.Terminate = 1;
				ServiceCMX7341Tx();
				ServiceAMBETx();
				if(Radio.Terminate == 0)
					Radio.Frame = 7;
			}
		}

	}
	Radio.Frame = 0;



	//setup code
	Spi_Write_byte_cmx7341(TX_FINE_CNTRL,0x01); // Tx fine control
	Spi_Read_byte_cmx7341(RX_FIFO_BYTE); // Flush Rx FIFO data

	Spi_Write_HalfWord(MODE_CTRL,0x0000); //Put into idle mode to load prog reg
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG); //flush status flags from earlier - especially prog
	check_status = 0;
	Spi_Write_HalfWord(PROG_ADDRESS,0x0601); //Select CMX994 using P6.1
	Spi_Write_HalfWord(PROG_REG,0x8012); //CMX7341 Default $8012

	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG)));

	Spi_Write_HalfWord(PROG_ADDRESS,0x0600); //Select RXIQ CMX7341 defaults to Rx IQ
	//Spi_Write_HalfWord(PROG_REG,0x3001); //Turn IQ AGC off - with $3005
	Spi_Write_HalfWord(PROG_REG,0x0009); //Turn IQ AGC off - with $3005

	HAL_Delay(10);

	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG)));

	HAL_Delay(10);

	Spi_Write_HalfWord(0xC0,0xFFC4); //power up analog hardware
	Spi_Write_HalfWord(0xB0,0x7700); //0dB MOD1 & MOD2, Outputs true
	/* Disc1->Input1, DAC2->MOD1
	; Disc2->Input2, DAC2->MOD2
	; DAC1->Audio
	; The gain setting included for test debug purposes,Fi takes control of gain with AGC On
	; 19.2dB ($DBD8),16dB ($B7D8),12.8dB ($93D8),9.6dB ($6FD8), 6.4dB ($4BD8) gain on both inputs */
	Spi_Write_HalfWord(0xB1,0xDBD8);
	//Spi_Write_HalfWord(0xB1,0x03D8);

	freq_cntrl = (0x8000 | 0x1E3);

	Spi_Write_HalfWord(CBUS_AUXDAC4,freq_cntrl);

	LoadRampProfile();

	ConfigTxSequence();

	Spi_Write_HalfWord(MODE_CTRL,0x0000); //;Put into idle mode to load prog reg
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG); //flush status flags from earlier - especially prog

	Spi_Write_HalfWord(PROG_ADDRESS,0x0602);//CMX994 Variant selection P.3.2
	Spi_Write_HalfWord(PROG_REG,0x0200);
	wait_status();

	config_CMX994_Rx();

	//Rx Calibration
	Spi_Write_HalfWord(MODE_CTRL,0x0000);
	while (SPI_Read_HalfWord(RX_FIFO_LEVEL) > 0)
	{
		//rx_fifo_byte = SPI_Read_HalfWord(RX_FIFO_BYTE);
		SPI_Read_HalfWord(RX_FIFO_BYTE);
	}

	Spi_Write_HalfWord(MODE_CTRL,0x001F);

	check_status = 0;
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);
	check_status = check_status & 0x4000;
	while(check_status != 0x4000)
	{
		check_status = SPI_Read_HalfWord(IRQ_STATUS_REG);
		check_status = check_status & 0x4000;
	}

	//while ((SPI_Read_HalfWord(IRQ_STATUS_REG)) !& (0x4000));
	SPI_Read_HalfWord(0xB9);

	Spi_Write_HalfWord(MODE_CTRL,0x0012);


	//Rx code
	Spi_Write_HalfWord(MODE_CTRL,0x0000);

	// Empty the RX FIFO before we start

	while (SPI_Read_HalfWord(RX_FIFO_LEVEL) > 0)
	{
		Spi_Read_byte_cmx7341(RX_FIFO_BYTE);
	}

	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG); //flush status flags from earlier - especially prog
	wait_status();

	Spi_Write_HalfWord(INTERRUPT_MASK, (IRQ | EVENT | TX_READY | RX_READY | FRAMESYNC) );

	HAL_Delay(100);

	Spi_Write_HalfWord(MODEM_CFG,0x03FF); // Enable all SYNCs with maximum error tolerance
	/* Put the Modem Control into:
	   TX = 4FSK Formatted, continuous (BS Mode)
	   RX = 4FSK Formatted, continuous (BS Mode)*/
	//Spi_Write_HalfWord(0xCE,0xFFFF);
	//HAL_Delay(1);
	Spi_Write_HalfWord(MODE_CTRL,0x0033); // 0x0022 for 4FSK RAW & 0x0033 4FSK DMR

	//HAL_UART_Transmit(&huart1, "setup DMR Rx\n\r", 14, 10);
	printf("setup DMR Rx\n\r");

}
void InitFI2DMRTx (void)
{
	//setup code
	Spi_Write_byte_cmx7341(TX_FINE_CNTRL,0x01); // Tx fine control
	Spi_Read_byte_cmx7341(RX_FIFO_BYTE); // Flush Rx FIFO data

	Spi_Write_HalfWord(MODE_CTRL,0x0000); //Put into idle mode to load prog reg

	//Spi_Write_HalfWord(INTERRUPT_MASK, (IRQ | EVENT | TX_READY | RX_READY | FRAMESYNC) );
	check_status = SPI_Read_HalfWord(IRQ_STATUS_REG); //flush status flags from earlier - especially prog
	check_status = 0;
	Spi_Write_HalfWord(PROG_ADDRESS,0x0601); //Select CMX994 using P6.1
	Spi_Write_HalfWord(PROG_REG,0x8012); //CMX7341 Default $8012

	//SPI Codec control
	Spi_Write_HalfWord(SPI_CODEC_CTRL,0x0012);

	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG)));

	Spi_Write_HalfWord(PROG_ADDRESS,0x0600); //Select RXIQ CMX7341 defaults to Rx IQ
	Spi_Write_HalfWord(PROG_REG,0x3001); //Turn IQ AGC off - with $3005

	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG)));

	HAL_Delay(10);

	Spi_Write_HalfWord(POWERDOWN_CTRL,0xFFC4); //power up analog hardware
	Spi_Write_HalfWord(ANALOG_OP_GAIN,0x7700); //0dB MOD1 & MOD2, Outputs true
	/* Disc1->Input1, DAC2->MOD1
		; Disc2->Input2, DAC2->MOD2
		; DAC1->Audio
		; The gain setting included for test debug purposes,Fi takes control of gain with AGC On
		; 19.2dB ($DBD8),16dB ($B7D8),12.8dB ($93D8),9.6dB ($6FD8), 6.4dB ($4BD8) gain on both inputs */
	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0xDBD8);
	//Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0xFFD8);
	SPI_Read_HalfWord(IRQ_STATUS_REG);


	freq_cntrl = (0x8000 | 0x1E3);

	Spi_Write_HalfWord(CBUS_AUXDAC4,freq_cntrl);

	LoadRampProfile();

	ConfigTxSequence();

	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x03B0); //0x03F0
	config_CMX994_Tx();
	Test_Pattern_2pmod();

	Spi_Write_HalfWord(INTERRUPT_MASK, (IRQ | EVENT | TX_READY | RX_READY | FRAMESYNC) );

	HAL_Delay(100);

	//Tx code
	Spi_Write_HalfWord(MODEM_CTRL,0x0000); //Put into idle mode to start with
	while(!(check_status = SPI_Read_HalfWord(IRQ_STATUS_REG))); //Flush the status register


	printf("setup DMR Tx\n\r");

	return;
}

void InitFI2DMR (void)
{
	uint32_t count;

	Radio.RadioState = START_DMR_RX;
	Radio.TxMode =  TXMODE_VOICE;
	cmx7341.AudioAttn = DEFAULT_AUDIOATTN;

	Spi_Write_HalfWord(POWERDOWN_CTRL,0xffc4); 	//power up everything
	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE, 0xFFD8);	//Disc->IP1, ALT->IP2 ; IP1 Gain = IP2 Gain = 0dB
	//OP1->Audio, Bias->MOD1, Bias->MOD2

	Spi_Write_HalfWord(MODE_CONTROL,0x0000);	//idle

	SPI_Read_HalfWord(STATUS);				//flush status
	cmx7341.Status = 0;
	cmx7341.IRQn = 0;

	ProgWrite(0x0600,0x8001);	//P6.0 select Tx 2-pt, RX IQ


	ProgWrite(0x0601,0x8012);	//P6.1 994 & SPI codec on separate ports; SPI codec mode.

	//if (cmx7341.Param.CMX994Type == CMX994E_TYPE)
	ProgWrite(0x0602,0x0010);	//modem defaults to 994A; if we have a 994, tell it.

	Spi_Write_HalfWord(AUX_DAC4_DATA,cmx7341.Param.FreqControl | 0x8000);	//AFC.

	LoadRampProfile();  								//load ramp profile
	cmx7341.RamdacScaling = DEFAULT_RAMDACSCALING;
	Spi_Write_HalfWord(RAMDAC_ATTEN,cmx7341.RamdacScaling);		//ramdac scaling

	ProgWrite(0x030D,0x0718);	//P3.13 set ramdac scan rate

	Spi_Write_HalfWord(AUX_DAC1_DATA,0x8000);					//AuxDAC1 to 0 in Rx.

	ConfigTxSequence();								//load tx sequence

	ConfigCMX994();

	SetAOGDMR();	//set up mod and audio gains.

	Spi_Write_HalfWord(INTERRUPT_MASK, (IRQ | EVENT | TX_READY | RX_READY | FRAMESYNC) );	//enable IRQs from CMX7341

	count = ReadCounter();						//let analogue hardware settle
	while (ReadCounter() < count + 500);

	Spi_Write_HalfWord(MODE_CONTROL,0x001f);	//rx iqdc cal

	while((SPI_Read_HalfWord(STATUS) & 0x4000) == 0);	//wait until done
	cmx7341.Param.CMX994DcOffset = SPI_Read_HalfWord(DC_OFFSET);

	return;
}

void  InitFI1Analogue (void)
{
	uint32_t count;
	uint16_t IChannel,QChannel;

	Radio.RadioState = START_ANALOGUE_RX;
	Radio.TxMode =  TXMODE_VOICE;
	cmx7341.AudioAttn = DEFAULT_AUDIOATTN;

	Spi_Write_HalfWord(POWERDOWN_CTRL,0xffc4); 	//power up everything

	Spi_Write_HalfWord(MODE_CONTROL,0x0000);	//idle

	SPI_Read_HalfWord(STATUS);				//flush status
	cmx7341.Status = 0;
	cmx7341.IRQn = 0;

	Spi_Write_HalfWord(ANALOG_LEVEL,0x20B0);             //Set Tx Voice Level to just limit

	Spi_Write_HalfWord(AUX_DAC1_DATA,0x8000);    //Enable AUXDAC1 with 0 value (PA ramp = 0)

	Spi_Write_HalfWord(IP_GAIN_SIG_ROUTE,0x9058);    //set routing, IP1 > DISC, IP2 > ALT, MIC +12.8dB in - MOD1 & MOD2 > Vbias
	Spi_Write_HalfWord(AUX_DAC1_DATA,0x0000);        //turn off DAC1 / PA

	Analogue_ProgBlock0();
	Analogue_ProgBlock2();
	ProgBlock6();                             //same as dPMR
	AgcConfig();

	Spi_Write_HalfWord(AUX_DAC4_DATA,cmx7341.Param.FreqControl | 0x8000);	//AFC.

	LoadRampProfile();  							//load ramp profile.
	cmx7341.RamdacScaling = DEFAULT_RAMDACSCALING;       //RAMDAC scaling factor for FI-1 Analogue
	Spi_Write_HalfWord(RAMDAC_ATTEN,cmx7341.RamdacScaling);		//ramdac scaling

	ConfigCMX994();          //same as dPMR

	SetAOGAnalogue();	           //set up mod and audio gains.

	Spi_Write_HalfWord(AUX_CONFIG,0x3070);           // setting read back iqdc

	count = ReadCounter();	//let analogue hardware settle

	while (ReadCounter() < count + 150000);

	Spi_Write_HalfWord(MODE_CONTROL,0x0015);	//rx iqdc cal

	while((SPI_Read_HalfWord(STATUS) & 0x4000) == 0);	//wait until done

	cmx7341.Param.CMX994DcOffset = SPI_Read_HalfWord(AUXDATA1);      // read back from $A9
	if (cmx7341.Param.CMX994Type == 0)   //CMX994
	{
		cmx7341.Param.CMX994DcOffset = (cmx7341.Param.CMX994DcOffset & 0xff);
	}
	else              //CMX994 A/E
	{
		cmx7341.Param.CMX994DcOffset = (cmx7341.Param.CMX994DcOffset & 0xfff);
		IChannel = (cmx7341.Param.CMX994DcOffset & 0x3f);
		QChannel = ((cmx7341.Param.CMX994DcOffset & 0xfc0) << 2);
		cmx7341.Param.CMX994DcOffset = (IChannel | QChannel);
	}
	return;

}

#define PAGE_ADDR       0x01/* Page number */


void ConfigCMX994(void)
{
	ProgWrite(0x0f11, 0x80); //Disable PLL - 80h to use ext LO and PLL
	ProgWrite(0x0f25, 0x10);	//VCO Control Reg

	if (cmx7341.Mode == MODE_dPMR)
		ProgWrite(0x0f12, 0x00);	//Rx Cntrl Reg div by 2 & Min BW ACR filter
	else
		ProgWrite(0x0f12, 0x08);	//Rx Cntrl Reg div by 2 & Mid BW ACR filter

	ProgWrite(0x0f16, 0x08);	//Rx gain Reg. Gain = Max. 00 = Max gain in 100R mode and 08 = max gain in 50R mode
	ProgWrite(0x0f14, 0x3F);	//IM Reg. 3F for best IM performance but slightly more gain at 00hex
	return;
}


void ConfigTxSequence_dPMR(void)
{
	while((SPI_Read_HalfWord(STATUS) & PROGBIT) == 0);
	Spi_Write_HalfWord(PROG_ADR, 0x0302);	//Start writing tx sequencer at P.3.2

	NextProgWrite(0x0000);	//TXENA switch-on time -        P.3.2 (default $0000)
	NextProgWrite(0x0000);	//RAMDAC ramp-up start time -   P.3.3 (default $0000)
	NextProgWrite(0x0000);	//GPIOA switch-on time -        P.3.4 (default $0000)
	NextProgWrite(0x0000);	//GPIOB switch-on time -        P.3.5 (default $0000)
	NextProgWrite(0x0050);	//Modulation start time -       P.3.6 (default $0050)
	NextProgWrite(0x0000);	//reserved    -                 P.3.7 (default $0000)
	NextProgWrite(0x001E);	//RAMDAC ramp-off start time -   P.3.8 (default $001E)
	NextProgWrite(0x006E);	//TXENA switch-off time -       P.3.9 (default $006E)
	NextProgWrite(0x0000);	//GPIOB switch-off time -       P.3.10 (default $0000)
	NextProgWrite(0x0000);	//GPIOA switch-off time -       P.3.11 (default $0000)

	return;
}
