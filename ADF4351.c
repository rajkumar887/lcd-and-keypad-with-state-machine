#include "stdio.h"                  // Device header
#include "stdlib.h"
#include "adf4351.h"
#include "stdint.h"
#include "usart.h"
#include "stm32f1xx_hal.h"
#include "math.h"
#include "Analogue.h"
#include "DMR.h"
#include "function_image.h"
#include "radio.h"
/**
 * \brief Buffer for ADF4351 registers
 *
 */
//volatile uint32_t ADF4351_Reg[6];


/**
 * \brief ADF4351 registers - local storage
 *
 */
ADF4351_Reg0_t ADF4351_Reg0;
ADF4351_Reg1_t ADF4351_Reg1;
ADF4351_Reg2_t ADF4351_Reg2;
ADF4351_Reg3_t ADF4351_Reg3;
ADF4351_Reg4_t ADF4351_Reg4;
ADF4351_Reg5_t ADF4351_Reg5;

typedef enum
{
	WAITING_FOR_FIRST_BYTE = 0,
	WAITING_FOR_SECOND_BYTE,
	WAITING_FOR_LENGTH,
	WAITING_FOR_TYPE,
	PAYLOAD,
} ADF4351_Type;

typedef struct
{
	uint16_t length;
	uint8_t type;
	uint8_t data[9];

} ADF4351PacketType;

int updateFrequency;
ADF4351PacketType ADF4351;

extern uint8_t RxChar1;
extern bool DMR_Mode;
double calcfreq;

void Set_Commands(uint8_t byte);
ADF4351_ERR_t UpdateFrequencyRegisters(double RFout, double REFin, double OutputChannelSpacing, int gcd, int AutoBandSelectClock, double *RFoutCalc );

/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1,&RxChar1,1);
	__HAL_UART_CLEAR_OREFLAG(&huart1);
	ADF4351_Set_Frequency(RxChar1);
	//HAL_UART_Transmit(&huart1,&RxChar,1,100);
}*/

void Set_Commands(uint8_t byte)
{
	static ADF4351_Type state = WAITING_FOR_FIRST_BYTE;
	static uint16_t  index=0;

	switch (state)
	{
	case WAITING_FOR_FIRST_BYTE:
		if (byte == 0x41)
			state = WAITING_FOR_SECOND_BYTE;
		break;

	case WAITING_FOR_SECOND_BYTE:
		if(byte == 0x54)
			state = WAITING_FOR_LENGTH;
		break;

	case WAITING_FOR_LENGTH:
		ADF4351.length = byte-48;
		state = WAITING_FOR_TYPE;
		break;

	case WAITING_FOR_TYPE:
		ADF4351.type = byte;
		state = PAYLOAD;
		break;

	case PAYLOAD:
		ADF4351.data[index++] = byte;

		if (index == ADF4351.length)
		{
			if(ADF4351.type == 0x66)
			{
				sscanf(ADF4351.data, "%d", &updateFrequency);
				UpdateFrequencyRegisters(updateFrequency, 38400000.0, 1000, 1, 1, &calcfreq);
				PLL_Sync();
				PLL_LD_Check(1000);
			}
			if(ADF4351.type == 0x46)
			{
				if(ADF4351.data[0] == 0x4D && ADF4351.data[1] == 0x4F && ADF4351.data[2] == 0x4E)
				{
					Radio.RadioState = START_ANALOGUE_TX;
				}
				if(ADF4351.data[0] == 0x4D && ADF4351.data[1] == 0x4F  && ADF4351.data[2] == 0x46 && ADF4351.data[3] == 0x46)
				{
					Radio.Terminate = 1;
				}
			}
			if(ADF4351.type == 0x44)
			{
				if(ADF4351.data[0] == 0x4D && ADF4351.data[1] == 0x52 && ADF4351.data[2] == 0x4F && ADF4351.data[3] == 0x4E)
				{
					Radio.RadioState = START_DMR_TX;

				}
				if(ADF4351.data[0] == 0x4D && ADF4351.data[1] == 0x52 && ADF4351.data[2] == 0x4F && ADF4351.data[3] == 0x46 && ADF4351.data[3] == 0x46)
				{
					Radio.Terminate = 1;
				}
			}
			index = 0;
			state = WAITING_FOR_FIRST_BYTE;
		}
		break;

	default:
		break;
	}
}


/**  \brief Private Function Prototypes
 *
 */

void PLL_IO_Init(void)
{
	PLL_CE_H;//PLL_CE = HIG;
	PLL_CK_H;//PLL_CK = HIG;
	PLL_DA_H;//PLL_DA = HIG;
	PLL_LE_H;//PLL_LE = HIG;

}

void SPI_SendWord(uint32_t data)
{
	int16_t i;
	PLL_LE_L;//PLL_LE = LOW;
	PLL_CK_L;//PLL_CK = LOW;

	for(i=0;i<32;i++)
	{
		if((data&0x80000000)!=0)
			PLL_DA_H;//PLL_DA = HIG;
		else
			PLL_DA_L;//PLL_DA = LOW;
		PLL_CK_H;//PLL_CK = HIG;
		PLL_CK_L;//PLL_CK = LOW;
		data <<= 1;
	}
	PLL_LE_H;//PLL_LE = HIG;
	PLL_CK_H;//PLL_CK = HIG;
	PLL_DA_H;//PLL_DA = HIG;

}

/***************************************************************************//**
 * @brief Writes 4 bytes of data to ADF4351.
 * @param data - Data value to write.
 * @retval SPI status
 *******************************************************************************/
void adf4351_write(uint32_t data)
{
	SPI_SendWord(data);
}


void PLL_Sync(void)
{
	int i;

	for (i=0; i<6; i++)
	{
		adf4351_write(ADF4351_GetRegisterBuf(5-i));	//	Write registers to PLL chip
	}
}
ADF4351_RFDIV_t ADF4351_Select_Output_Divider(double RFoutFrequency)
{
	// Select divider
	if (RFoutFrequency < 137500000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 5;
		return ADF4351_RFDIV_32;
	}
	if (RFoutFrequency < 275000000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 4;
		return ADF4351_RFDIV_16;
	}
	if (RFoutFrequency < 550000000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 3;
		return ADF4351_RFDIV_8;
	}
	if (RFoutFrequency < 1100000000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 2;
		return ADF4351_RFDIV_4;
	}
	if (RFoutFrequency < 2200000000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 1;
		return ADF4351_RFDIV_2;
	}
	if (RFoutFrequency >= 2200000000.0)
	{
		ADF4351_Reg4.b.RfDivSel = 0;
		return ADF4351_RFDIV_1;
	}
	else
	{
		ADF4351_Reg4.b.RfDivSel = 6;
		return ADF4351_RFDIV_64;
	}
}

// greatest common denominator - euclid algo w/o recursion
int gcd_iter(uint32_t u, uint32_t v) {
	uint32_t t;
	while (v) {
		t = u;
		u = v;
		v = t % v;
	}
	return u ;
}


/**
 *  \brief Main function to calculate register values based on required PLL output
 *
 * @param  RFout: 								Required PLL output frequency in Hz
 * 				REFin:								Reference clock in Hz
 * 				OutputChannelSpacing:	Output channel spacing in Hz
 * 				gcd:									calculate CGD (1) or not (0)
 * 				AutoBandSelectClock:	automatic calc of band selection clock
 * @paramOut  RFoutCalc: 				Calculated actual output frequency in Hz
 * @retval 0=OK, or Error code (ADF4351_ERR_t)
 */
ADF4351_ERR_t UpdateFrequencyRegisters(double RFout, double REFin, double OutputChannelSpacing, int gcd, int AutoBandSelectClock, double *RFoutCalc )
{
	uint16_t 		OutputDivider;
	uint32_t 		temp;
	double			PFDFreq;							// PFD Frequency in Hz
	uint16_t 		Rcounter;							// R counter value
	int 				RefDoubler;  					// ref. doubler
	int 				RefD2;			 					// ref. div 2
	double 			N;
	uint16_t		INT,MOD,FRAC;
	uint32_t    D;
	double 			BandSelectClockDivider;
	double 			BandSelectClockFrequency;

	RFout = RFout * 2;

	/** Initial error and range check */
	/* Error >>>> Disable GCD calculation when phase adjust active */
	//if (gcd && ADF4351_Reg1.b.PhaseAdjust) return ADF4351_Err_NoGCD_PhaseAdj;
	if (RFout > ADF4351_RFOUT_MAX) return	ADF4351_Err_RFoutTooHigh;
	if (RFout < ADF4351_RFOUTMIN) return ADF4351_Err_RFoutTooLow;
	if (REFin > ADF4351_REFINMAX) return	ADF4351_Err_REFinTooHigh;

	// Calculate N, INT, FRAC, MOD

	RefD2 = ADF4351_Reg2.b.RDiv2 + 1;					// 1 or 2
	RefDoubler = ADF4351_Reg2.b.RMul2 + 1;     // 1 or 2
	Rcounter = ADF4351_Reg2.b.RCountVal;
	PFDFreq = (REFin * RefDoubler / RefD2) / Rcounter;

	OutputDivider = ADF4351_Select_Output_Divider(RFout);


	if (ADF4351_Reg4.b.Feedback == 1) // fundamental
		N = ((RFout * OutputDivider) / PFDFreq);
	else										// divided
		N = (RFout / PFDFreq);

	INT = (uint16_t)N;
	MOD = (uint16_t)(round(/*1000 * */(PFDFreq / OutputChannelSpacing)));
	FRAC = (uint16_t)(round(((double)N - INT) * MOD));

	if (gcd)
	{
		D = gcd_iter((uint32_t)MOD, (uint32_t)FRAC);

		MOD = MOD / D;
		FRAC = FRAC / D;
	}

	if (MOD == 1)
		MOD = 2;

	*RFoutCalc = (((double)((double)INT + ((double)FRAC / (double)MOD)) * (double)PFDFreq / OutputDivider) * ((ADF4351_Reg4.b.Feedback == 1) ? 1 : OutputDivider));
	N = INT + (FRAC / MOD);

	// N is out of range!
	if ((N < 23) | (N > 65635)) return ADF4351_Err_InvalidN;
	if (MOD > 0x0fff) return ADF4351_Err_InvalidMOD;

	/* Check for PFD Max error, return error code if not OK */
	if ((PFDFreq > ADF5451_PFD_MAX) && (ADF4351_Reg3.b.BandSelMode == 0)) return ADF4351_Err_PFD;
	if ((PFDFreq > ADF5451_PFD_MAX) && (ADF4351_Reg3.b.BandSelMode == 1) && (FRAC != 0)) return ADF4351_Err_PFD;
	if ((PFDFreq > 90) && (ADF4351_Reg3.b.BandSelMode == 1) && (FRAC != 0))  return ADF4351_Err_PFD;
	if ((ADF4351_Reg2.b.LowNoiseSpur == ADF4351_LOW_SPUR_MODE) && (MOD < 50)) return ADF4351_Err_InvalidMODLowSpur;

	//		Calculate Band Select Clock
	if (AutoBandSelectClock)
	{
		if (ADF4351_Reg3.b.BandSelMode == 0)   /// LOW
		{
			temp = (uint32_t)round(8 * PFDFreq);
			if ((8 * PFDFreq - temp) > 0)
				temp++;
			temp = (temp > 255) ? 255 : temp;
			BandSelectClockDivider = 0x9A; //(double)temp;
		}
		else                                            // High
		{
			temp = (uint32_t)round(PFDFreq * 2);
			if ((2 * PFDFreq - temp) > 0)
				temp++;
			temp = (temp > 255) ? 255 : temp;
			BandSelectClockDivider = (double)temp;
		}
	}
	BandSelectClockFrequency = (PFDFreq / (uint32_t)BandSelectClockDivider);

	/* Check parameters */
	if (BandSelectClockFrequency > 500e3)  return ADF4351_Err_BandSelFreqTooHigh;  // 500kHz in fast mode
	if ((BandSelectClockFrequency > 125e3) & (ADF4351_Reg3.b.BandSelMode == 0))  return ADF4351_Err_BandSelFreqTooHigh;   // 125kHz in slow mode

	// So far so good, let's fill the registers

	ADF4351_Reg0.b.FracVal = (FRAC & 0x0fff);
	ADF4351_Reg0.b.IntVal = (INT & 0xffff);
	ADF4351_Reg1.b.ModVal = (MOD & 0x0fff);
	ADF4351_Reg4.b.BandClkDiv = BandSelectClockDivider;

	//ADF4351_Reg4.w = 0xDD003C;

	if (*RFoutCalc == RFout)
		return ADF4351_Err_None;
	else
		return ADF4351_Warn_NotTuned;			// PLL could not be tuned to exatly required frequency --- check the RFoutCalc foree exact value
}


/**
 *  \brief Returns current value from local register buffer
 *
 * @param  addr: 								Register address
 * @retval register value
 */
uint32_t ADF4351_GetRegisterBuf(int addr)
{
	switch (addr)
	{
	case 0 : return ADF4351_Reg0.w;
	case 1 : return ADF4351_Reg1.w;
	case 2 : return ADF4351_Reg2.w;
	case 3 : return ADF4351_Reg3.w;
	case 4 : return ADF4351_Reg4.w;
	case 5 : return ADF4351_Reg5.w;
	}
	return 0x00000007;			// invalid address
}

/**
 *  \brief Set local register buffer value
 *
 * @param  addr: 								Register address
 */
void ADF4351_SetRegisterBuf(int addr, uint32_t val)
{
	switch (addr)
	{
	case 0 : ADF4351_Reg0.w = val;
	case 1 : ADF4351_Reg1.w = val;
	case 2 : ADF4351_Reg2.w = val;
	case 3 : ADF4351_Reg3.w = val;
	case 4 : ADF4351_Reg4.w = val;
	case 5 : ADF4351_Reg5.w = val;
	}
}


/**
 *  \brief Clear local register buffer values to 0
 *
 * @param  none
 */
void ADF4351_ClearRegisterBuf(void)
{
	int i;

	for (i=0; i<6; i++) ADF4351_SetRegisterBuf(i, 0x00000000);

}



/**
 *  \brief Set R counter value
 *
 * @param  val: 								Counter value (1...1023)
 * @retval register value
 */
ADF4351_ERR_t ADF4351_SetRcounterVal(uint16_t val)
{
	if ((val>0) & (val<1024))
	{
		ADF4351_Reg2.b.RCountVal = val;
		return ADF4351_Err_None;
	}
	else
		return ADF4351_Err_InvalidRCounterValue;
}


/**
 *  \brief Init ADF4351 registers
 *
 * @param  none
 */
ADF4351_ERR_t ADF4351_Init(void)
{
	PLL_IO_Init();
	ADF4351_Reg0.b.ControlBits = 0U;
	ADF4351_Reg1.b.ControlBits = 1U;
	ADF4351_Reg2.b.ControlBits = 2U;
	ADF4351_Reg3.b.ControlBits = 3U;
	ADF4351_Reg4.b.ControlBits = 4U;
	ADF4351_Reg5.b.ControlBits = 5U;
	ADF4351_Reg5.b._reserved_1 = 3U;
	return ADF4351_Err_None;
}

void ADF4351_Reg_Config(void)
{
	ADF4351_Reg1.b.PhaseAdjust = ADF4351_DISABLE;
	ADF4351_Reg1.b.PhaseVal = 1;
	ADF4351_Reg1.b.Prescaler = ADF4351_PRESCALER_4_5;

	ADF4351_Reg2.b.CounterReset = ADF4351_DISABLE;
	ADF4351_Reg2.b.RDiv2 = ADF4351_REFDIV_1;
	ADF4351_Reg2.b.RMul2 = ADF4351_REFMUL_1;
	ADF4351_Reg2.b.CPCurrent = ADF4351_CPCURRENT_1_25;
	ADF4351_Reg2.b.CPTristate = ADF4351_DISABLE;
	ADF4351_Reg2.b.DoubleBuffer  = ADF4351_DISABLE;
	ADF4351_Reg2.b.LockFunction = ADF4351_LDF_FRAC;
	ADF4351_Reg2.b.LockPrecision = ADF4351_LDP_10NS;
	ADF4351_Reg2.b.LowNoiseSpur = ADF4351_LOW_NOISE_MODE;
	ADF4351_Reg2.b.MuxOut = ADF4351_MUX_DIGITALLOCK;
	ADF4351_Reg2.b.PhasePolarity = ADF4351_POLARITY_POSITIVE;
	ADF4351_Reg2.b.PowerDown = ADF4351_DISABLE;
	ADF4351_Reg2.b.RCountVal = 2;

	ADF4351_Reg3.b.AntibacklashW = ADF4351_ABP_6NS;
	ADF4351_Reg3.b.BandSelMode = ADF4351_BANDCLOCK_LOW;
	ADF4351_Reg3.b.ChargeCh = ADF4351_DISABLE;
	ADF4351_Reg3.b.ClkDivMod = ADF4351_CLKDIVMODE_OFF;
	ADF4351_Reg3.b.ClkDivVal = 150;
	ADF4351_Reg3.b.CsrEn = ADF4351_DISABLE;

	ADF4351_Reg4.b.AuxEnable = ADF4351_DISABLE;
	ADF4351_Reg4.b.Mtld = ADF4351_DISABLE;
	ADF4351_Reg4.b.OutEnable = ADF4351_ENABLE;
	ADF4351_Reg4.b.OutPower = ADF4351_POWER_PLUS5DB;
	ADF4351_Reg4.b.VcoPowerDown =  ADF4351_DISABLE;
	ADF4351_Reg4.b.Feedback = ADF4351_FEEDBACK_FUNDAMENTAL;

	ADF4351_Reg4.b.AuxPower	= ADF4351_POWER_MINUS4DB; //		:2;  		/*!< bit:  6.. 7 	AUX Output Power */
	ADF4351_Reg4.b.AuxSel = ADF4351_FEEDBACK_DIVIDED; //				:1;  		/*!< bit:  9 	AUX Output Select */
	//ADF4351_Reg4.b.RfDivSel = 5;

	ADF4351_Reg5.b.LdPinMode = ADF4351_LD_PIN_DIGITAL_LOCK;
}

void sleep(uint32_t cnt)
{
	HAL_Delay(cnt);
}

int8_t PLL_LD_Check(uint16_t count)
{
	uint16_t i;
	int8_t OK_NG=_NO_ERROR;
	for(i = 0; i<count ;i++)
	{
		if (PLL_LD())	//Lock
		{
			OK_NG = _NO_ERROR;
			HAL_UART_Transmit(&huart1,(uint8_t*)"PLL Lock done\n\r",15,100);
			return OK_NG;
		}
		else
		{
			OK_NG = _ERROR;
			HAL_UART_Transmit(&huart1,(uint8_t*)"PLL Lock failed\n\r",17,100);
			//sleep(3);			//300us
		}
	}
	return OK_NG;
}
