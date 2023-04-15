#include "stdint.h"

void InitRadio(void);
void DoRadio(void);
void SetAOGDMR(void);
void SetAOGdPMR(void);
void SetAOGAnalogue(void);
void InitFI2DMR(void);
void InitFI1dPMR(void);
void InitFI1Analogue (void);
void GoToDMR(void);
void GoTodPMR(void);
void GoToAnalogue(void);
void NoModeChange(void);
void LoadEEPROMParameters(void);
void InitFI2DMRTx (void);

typedef enum
{
	START_DMR_RX = 0,
	DMR_RX_IDLE,
	DMR_RX_ACTIVE,
	DMR_RX_DISPLAY_MESSAGE,
	START_DMR_TX,
	DMR_TX,
	START_ANALOGUE_RX,
	ANALOGUE_RX_IDLE,
	ANALOGUE_RX_ACTIVE,
	START_ANALOGUE_TX,
	ANALOGUE_TX,
	RADIO_IDLE,	//used when MMI in config. DO we need thsi?

	START_dPMR_RX,
	dPMR_RX_IDLE,
	dPMR_RX_ACTIVE,
	dPMR_RX_DISPLAY_MESSAGE,
	START_dPMR_TX,
	dPMR_TX,
	dPMR_Tx_End,
	dPMR_Tx_STATUS_MESSAGE,
	dPMR_TX_STATUS_START,
	dPMR_TX_STATUS_SEND,
	dPMR_TX_STATUS_END,
	dPMR_TX_STATUS2_START,             //Status2 is for icom F-4262
	dPMR_TX_STATUS2_SEND,
	dPMR_TX_STATUS2_END,
} RadioStateType;

typedef enum
{
	TXMODE_VOICE = 0,
	TXMODE_AMBE_TEST_TONE,
	TXMODE_MOD_SETUP,
	TXMODE_PRBS,
	TXMODE_MSG1,
	TXMODE_MSG2,
	//TXMODE_PER,
}TxModeType;

typedef enum
{
	RXMODE_NORMAL = 0,
	RXMODE_PASSTHROUGH,
	RXMODE_EYE,
	//RXMODE_PER,
}RxModeType;


typedef struct
{
	RadioStateType RadioState;
	RadioStateType RadioSave;
	uint16_t TxMode;
	uint16_t RxMode;
	int16_t Channel;
	uint8_t Slot;
	uint8_t Header;
	uint8_t Terminate;
	uint8_t RC;
	uint16_t Frame;
	uint16_t Packets;
	uint16_t TxCount;
	uint16_t RxCount;
	uint8_t CodecOn;
	char	Message[110];
	char*   Msgptr;
	char*   EndOfMessage;
}RadioType;

extern RadioType Radio;

typedef struct
{
	char Name[5];
	uint8_t frac1;
    uint8_t frac2;
} ChannelType ;

typedef struct
{
	uint8_t  Tx[27*2];		//buffer for 2 voice frames - Hard bits, coded by AMBE for Tx by 7341
	uint8_t  Rx[108*2];		//buffer for 2 voice frames - Soft bits, from 7341 Rx, for decoding by AMBE
	uint16_t CMX7341Idx;	//indices into either buffer; start 1 frame apart, wrap when they reach the end.
	uint16_t AMBEIdx;
} VoiceFrameType;

extern VoiceFrameType VF;

typedef struct
{
	uint8_t BurstType;
	uint8_t BurstInfo;
	uint8_t Errors;
	uint8_t Timing;
	uint8_t Data[108];	//108 bytes in voice payload ; 216 soft metrics, each 4 bits, packed 2 per byte.
} VoicePacketType;
extern VoicePacketType VoicePacket;

typedef struct
{
	uint8_t BurstType;
	uint8_t BurstInfo;
	uint8_t Errors;
	uint8_t Timing;
	uint8_t ColourCode;
	uint8_t DataType;
	uint8_t Data[24];	//24 bytes max data payload
} DataPacketType;
extern DataPacketType DataPacket;


typedef struct
{
	uint8_t BurstType;
	uint8_t BurstInfo;
	uint8_t Errors;
	uint8_t Timing;
	uint8_t Data[10];	//24 bytes max data payload
} LCEMBPacketType;
extern LCEMBPacketType LCEMBPacket;

typedef struct
{
	uint8_t BurstType;
	uint8_t BurstInfo;
	uint8_t Errors;
	uint8_t Timing;
	uint8_t Data[3];	//3 bytes payload
} RCEMBPacketType;
extern RCEMBPacketType RCEMBPacket;

//Burst Types
#define CACH_TACT		2
#define CACH_SLC		3
#define LC_EMB			7
#define LC_VOICE_HDR	8
#define LC_VOICE_TERM	9
#define VOICE_PAYLOAD	10
#define DATA			11
#define EMB_DATA		14

//Data Types.
#define DT_PI_HDR		0
#define DT_CSBK 		3
#define DT_MBC_HDR		4
#define DT_MBC_CONT		5
#define DT_DATA_HDR		6
#define DT_RATE_12		7
#define DT_RATE_34		8
#define DT_IDLE			9
#define DT_RATE_1		10

#define MIN_DMR_CHANNEL 0
#define MAX_DMR_CHANNEL 7

#define MIN_dPMR_CHANNEL 8
#define MAX_dPMR_CHANNEL 23

#define MIN_ANALOGUE_CHANNEL 24
#define MAX_ANALOGUE_CHANNEL 31

#define MAX_CHANNEL_TABLE 31

#define HYSTERESIS  5

#define MIN_VOL 0
#define MAX_VOL 15

#define VOICE_TIMEOUT 12

#define FM_MODE 0
#define DMR_MODE 1
//#define IQTX	//test mode
