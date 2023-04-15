/************************************************************************************************************
 *
 *  $Copyright � 2015-2016 Applied Technology UK $
 *
 *  $Header: cmx7341.h  Revision:1.30  18 August 2016 14:11:23  ilewis $
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

/***** Definition of registers used for FI-2 ********************/

//****************************************************************
//               CBUS registers: FIFOs
//****************************************************************

#include "stdint.h"

#define SOFT_RESET   	  0x01
#define TXFIFO_BYTE   	  0x78
#define TXFIFO_WORD   	  0x79
#define TXFIFO_CTRL		  0x7A
#define TXFIFO_LEVEL      0x7B


#define TXFIFO_THRESHOLD  0x7A
#define TXFIFO_STATUS     0x7B

#define RXFIFO_BYTE       0x7C
#define RXFIFO_WORD       0x7D
#define RXFIFO_CTRL       0x7E
#define RXFIFO_LEVEL      0x7F

//****************************************************************
//               CBUS registers: host write
//****************************************************************
#define ANALOG_CTRL        0x80	//8-bit
#define RAMDAC_ATTEN       0x84
#define RAMDAC_OFFSET      0x85
#define AUX_ADC_CTRL       0x93
#define AUX_ADC1_THRESH_LO 0x94
#define AUX_ADC1_THRESH_HI 0x95
#define AUX_ADC2_THRESH_LO 0x96
#define AUX_ADC2_THRESH_HI 0x97
#define AUX_IRQ_STATUS     0x99

#define SPI_CODEC_CTRL     0xA0

#define CBUS_A1            0xA1
#define CBUS_A2            0xA2
#define CBUS_A3            0xA3
#define CBUS_A4            0xA4
#define CBUS_A5            0xA5
#define CBUS_A6            0xA6
#define CBUS_A7            0xA7
#define CBUS_A8            0xA8
#define SYSCLK1_PLL_CONFIG 0xAB
#define SYSCLK1_REF_CONFIG 0xAC
#define SYSCLK2_PLL_CONFIG 0xAD
#define SYSCLK2_REF_CONFIG 0xAE
#define BREAKPOINT         0xAF
#define ANALOG_OP_GAIN     0xB0

#define IP_GAIN_SIG_ROUTE  0xB1
#define RF_CHANNEL_DATA    0xB2
#define CBUS_B5            0xB5
#define CBUS_B6            0xB6
#define CBUS_B7            0xB7
#define DSP_PLL_CONFIG     0xBC

#define DSP_REF_CONFIG     0xBD

#define REGULATOR_ADJ      0xBF
#define POWERDOWN_CTRL     0xC0
#define MODE_CONTROL       0xC1
#define MODEM_CONFIG       0xC2

#define ANALOG_LEVEL       0xC3
#define PROG_ADR           0xC7
#define PROG_REG           0xC8

#define TIMING_CONTROL     0xCA
#define SETUP_PATTERN      0xCB
#define AUX_CONFIG         0xCD
#define INTERRUPT_MASK     0xCE
#define TEST_MODE_CTRL     0xCF

//****************************************************************
//               CBUS registers: host read
//****************************************************************
#define AUX_DAC1_DATA      0x30
#define AUX_DAC2_DATA      0x31
#define AUX_DAC3_DATA      0x32
#define AUX_DAC4_DATA      0x33
#define CBUS_98            0x98
#define CBUS_99            0x99
#define CBUS_9A            0x9A
#define CBUS_9B            0x9B
#define CBUS_9C            0x9C
#define CBUS_9D            0x9D
#define RSSI_AGC           0x9E
#define CBUS_9F            0x9F
#define XCHECK_HI          0xA9
#define XCHECK_LO          0xAA
#define PCHECK_HI          0xB8
#define PCHECK_LO          0xB9
#define DC_OFFSET		   0xB9
#define CBUS_BA            0xBA
#define CBUS_BB            0xBB
#define POWERDOWN_READ     0xC4
#define DEVICE_TYPE        0xC5

#define STATUS             0xC6
#define PROGBIT            0x0001

#define FRAMESYNC          0x0010
#define RX_READY           0x0020
#define TX_READY           0x0040

#define AUX_ADC1           0x0100
#define AUX_ADC2           0x0200

//#define RSSI               0x1000
#define WAKEUP             0x2000
#define EVENT              0x4000
#define IRQ          	   0x8000


#define FI_VERSION         0xC9
#define MODEM_STATUS       0xC9

#define SLOT_NUMBER        0xCC
#define FI2ACT1 0x50AA	//Activation codes CMX7341 FI-2
#define FI2ACT2 0xE5F6

/***** Definition of registers used for FI-1 ********************/


//***********CBUS registers: host write****************
#define MODEM_CONF_FI1     0xA1

//***********CBUS registers: host read****************
#define FS_STATUS		   0xC5

#define AUXDATA1		   0xA9
#define AUXDATA2		   0xAA

#define TXDATA0            0xB5
#define TXDATA1            0xB6
#define TXDATA2            0xB7
#define TXDATA3            0xCA
#define TXDATA4            0xCB

#define RXDATA0            0xB8
#define RXDATA1            0xB9
#define RXDATA2            0xBA
#define RXDATA3            0xBB
#define RXDATA4            0xC5

//STATUS BITS
#define RF_STATUS          0x0002
#define SEQ_EVENT          0x0004
#define INBAND             0x0008
#define DATA_RDY           0x0020
#define TX_DONE            0x0040
#define CALLED             0x0080
#define SLOW_DATA          0x0400
#define SUBAUDIO           0x0800
#define SQL_PS			   0x2000

#define SQL_RSSI           0x3000

//***********CMX994 REGISTERS : HOST WRITE ****************//
#define GEN_CTRL_REG	0x0F11
#define RX_CTRL_REG		0x0F12
#define RX_OFFSET_REG	0x0F13
#define OPT_CTRL_REG	0x0F15
#define RX_GAIN_REG		0x0F16
#define RX_OFFSET_REG1  0x0F17
#define VCO_CTRL_REG	0x0F25
#define GEN_CTRL_REG_R  0x0FE1

#define FI1ACT1 0xAB65	//Activation codes CMX7341 FI-1
#define FI1ACT2 0x7BAF
#define FRAME_LEN    36
#define BUFFER_LEN   (FRAME_LEN*6)


//Parameters stored in EEPROM
typedef struct
{
	 uint16_t CMX994DcOffset;	//redundant; calibrated by CMX7341 DON'T REMOVE AS IT WILL MESS UP 'OLDER' EEPROMS
	 uint16_t CoarseModAttnDMR;
	 uint16_t FineModAttnDMR;
	 uint16_t FreqControl;
	 uint16_t MicGain;
	 uint16_t CMX994Type;		//A,E or no suffix
	 uint16_t SquelchThresh;    //for Analogue mode
	 uint16_t Powersave;        //for Analogue and dPMR mode
	 //Only add new entries here, at the end!!!!!
	 uint16_t CoarseModAttndPMR;
	 uint16_t FineModAttndPMR;
	 uint16_t CoarseModAttnAnalogue;
	 uint16_t FineModAttnAnalogue;

}Param_Type;

typedef enum
{
	MODE_ANALOGUE = 0,
	MODE_dPMR,
	MODE_DMR,
}Mode_Type;

 typedef struct
 {
	 //cmxFI_TypeDef FI;
	 uint16_t FIVersion;	//read back after boot
	 Mode_Type Mode;
	 uint8_t  IRQn;
	 uint16_t Status;
	 uint8_t  AudioAttn;
	 uint16_t RamdacScaling;
	 union
	 {
		 Param_Type Param;								//parameters stored in eeprom
		 uint32_t ParamAlign[(sizeof(Param_Type)/4)+1];	//align to word boundary for EEPROM programming
	 };
 } CMX7341_Type;

 extern CMX7341_Type cmx7341;

//CMX994 Types
#define CMX994_TYPE  0x0000
#define CMX994A_TYPE 0x0001
#define CMX994E_TYPE 0x0002

//EEPROM parameter defaults
#define DEFAULT_CMX994DCOFFSET			0x00F3
#define DEFAULT_COARSEMODATTN_DMR 		0x6300
#define DEFAULT_FINEMODATTN_DMR			0x0037
#define DEFAULT_FREQCONTROL     		0x01D3
#define DEFAULT_MICGAIN    				0x0000
#define DEFAULT_CMX994TYPE				CMX994A_TYPE
#define DEFAULT_SQUELCH_THRESHOLD       2000
#define DEFAULT_POWERSAVE				0
#define DEFAULT_COARSEMODATTN_dPMR 		0x5300
#define DEFAULT_FINEMODATTN_dPMR		0x0087
#define DEFAULT_COARSEMODATTN_ANALOGUE 	0x6300
#define DEFAULT_FINEMODATTN_ANALOGUE	0x0037

//Non-EEPROM parameter defaults
#define DEFAULT_AUDIOATTN		10
#define RAMDAC_FULLPOWER		0x03ff
#define RAMDAC_LOWPOWER			0x0350 //0x0200
#define RAMDAC_OFF				0x0000
#define DEFAULT_RAMDACSCALING	RAMDAC_LOWPOWER

#define EEPROM_SIG				0x12345600	//first word in EEPROM (24msbs) to indicate valid data in EEPROM
