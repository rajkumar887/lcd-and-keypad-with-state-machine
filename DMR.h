/*
 * DMR.h
 *
 *  Created on: 03-Sep-2022
 *      Author: dmr1
 */

#ifndef INC_DMR_H_
#define INC_DMR_H_

#include "stdbool.h"

#define FM_MODE 0
#define DMR_MODE 1
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

#define RSSI               0x1000
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

#define FI1ACT1 0xAB65	//Activation codes CMX7341 FI-1
#define FI1ACT2 0x7BAF

#define MODE_CTRL	 	0xC1
#define MODEM_CFG	 	0xC2
#define IRQ_STATUS_REG     0xC6
#define TX_TEST_PATT		0xCB

#define CBUS_AUXDAC2       0x31
#define CBUS_AUXDAC4		0x33
#define TX_OP_CNTRL	 	0xB0
#define TX_FINE_CNTRL	 	0x80
#define RAMDAC_Level		0x84

#define RX_FIFO_BYTE 	 0x7C
#define RX_FIFO_WORD	 0x7D
#define RX_FIFO_LEVEL	 0x7F

#define acr_filt    	    2		//;CMX994 Default mid filter setting for DMR
#define rx_gain     	    1		//;CMX994 Default max gain
//;offset_reg  	word    0x9A		//;CMX994 Default value, user should enter a value for their particular board S/N:202589 = 9Ahex
//#define freq_cntrl			0x1D8;//1D3	//;Default (0x1D8 for 202596), user should enter a value for their particular board sn 202589
#define txrx_cfg    	    1
#define txsource	word	1
//#define temp		word	1
//#define temp1		word	0
#define CMX994_type	word	1 	;default to CMX994A (Note FI defaults to CMX994A from power up)
#define Rx_Cal		word	1 	;1 = Do Rx DC Offset Cal
#define txtest		word	0	;1 = Tx Mod setup
//#define offset994	word
//#define rx_fifo_byte 	word

#define PROG_READY  	   0x1
#define PROG_ADDRESS       0xC7
#define PROG_REG     	   0xC8

#define PROGBIT            0x0001


#define DEFAULT_NUMBER_OF_BURSTS	1000

#define TX_FIFO_BYTE    	 	0x78
#define TX_FIFO_WORD    	 	0x79
//   	register 1,TX_FIFO_BYTE,1
#define TX_FIFO_LEVEL   	 	0x7B
#define MODEM_CTRL		 0xC1
#define CBUS_SLOT_NUMBER 	 0xCC

#define IRQ_TX_RDY  		   0x0040
#define IRQ_TX_DONE  		   0x0004

//uint16_t uVoicePrbsIdx = 0,	uVoiceByteCnt =	0, uBurstExitFlag = 0, uBurstTimeSlot = 0, uBurstCnt =0;
//uint16_t uBurstTemp0 = 0, uBurstCntThreshold = 0;



void StartDMRTx(void);
void ServiceCMX7341Tx(void);
void ServiceAMBETx(void);

void DMRRx(void);
void start_DMR_Rx(void);
void DMR_Rx_Idle(void);
void DMR_Rx_Active(void);
#endif /* INC_DMR_H_ */
