#include "stdint.h"
/*
++++++++++++++++++++++++++++++++++++++++
+   Packet types
++++++++++++++++++++++++++++++++++++++++
*/
#define AMBE_PKT_CTRL           (0x00)
#define AMBE_PKT_CHAN           (0x01)
#define AMBE_PKT_SPEECH         (0x02)
/*
++++++++++++++++++++++++++++++++++++++++
+   Control packet field identifiers
++++++++++++++++++++++++++++++++++++++++
*/
#define AMBE_PKT_CHANNEL0       (0x40)
#define AMBE_PKT_ECMODE         (0x05)
#define AMBE_PKT_DCMODE         (0x06)
#define AMBE_PKT_COMPAND        (0x32)
#define AMBE_PKT_RATET          (0x09)
#define AMBE_PKT_RATEP          (0x0a)
#define AMBE_PKT_INIT           (0x0b)
#define AMBE_PKT_LOWPOWER       (0x10)
#define AMBE_PKT_CODECCFG       (0x38)
#define AMBE_PKT_CODECSTART     (0x2a)
#define     AMBE_CODECSTART_SPI_PTD     (0x00)
#define     AMBE_CODECSTART_SPI_PTE     (0x02)
#define     AMBE_CODECSTART_MCBSP_PTD   (0x04)
#define     AMBE_CODECSTART_MCBSP_PTE   (0x06)
#define AMBE_PKT_CODECSTOP      (0x2b)
#define AMBE_PKT_CHANFMT        (0x15)
#define AMBE_PKT_SPCHFMT        (0x16)
#define AMBE_PKT_PRODID         (0x30)
#define AMBE_PKT_VERSTRING      (0x31)
#define AMBE_PKT_READY          (0x39)
#define AMBE_PKT_HALT           (0x35)
#define AMBE_PKT_RESET          (0x33)
#define AMBE_PKT_RESETSOFTCFG   (0x34)
#define AMBE_PKT_GETCFG         (0x36)
#define AMBE_PKT_READCFG        (0x37)
#define AMBE_PKT_PARITYMODE     (0x3f)
#define AMBE_PKT_WRITEI2C       (0x44)
#define AMBE_PKT_CLRCODECRESET  (0x46)
#define AMBE_PKT_SETCODECRESET  (0x47)
#define AMBE_PKT_DISCARDCODEC   (0x48)
#define AMBE_PKT_DELAYNUS       (0x49)
#define AMBE_PKT_DELAYNNS       (0x4a)
#define AMBE_PKT_RTSTHRESH      (0x4e)
#define AMBE_PKT_GAIN           (0x4b)

//////////////////////////////////////
#define AMBE_CHAND				(0x01)
#define AMBE_CHAND4				(0x17)

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+   Configuration bits. We will store the config bits in a uint32_t
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define AMBE_CFG_IFSEL_SHIFT    (0)
#define AMBE_CFG_IFSEL_VALUE    (0x07)
#define AMBE_CFG_IFSEL_MASK     (AMBE_CFG_IFSEL_VALUE << AMBE_CFG_IFSEL_SHIFT)

#define AMBE_CFG_DTXENAB        (1 << 3)
#define AMBE_CFG_RSVD1          (1 << 4)
#define AMBE_CFG_NSENAB         (1 << 5)
#define AMBE_CFG_CPENAB         (1 << 6)
#define AMBE_CFG_CPSEL          (1 << 7)

#define AMBE_CFG_VRATE_SHIFT    (8)
#define AMBE_CFG_VRATE_VALUE    (0x3f)
#define AMBE_CFG_VRATE_MASK     (AMBE_CFG_VRATE_VALUE << AMBE_CFG_VRATE_SHIFT)

#define AMBE_CFG_ECENAB         (1 << 14)
#define AMBE_CFG_ESENAB         (1 << 15)

#define AMBE_CFG_BRATE_SHIFT    (16)
#define AMBE_CFG_BRATE_VALUE    (0x07)
#define AMBE_CFG_BRATE_MASK     (AMBE_CFG_BRATE_VALUE << AMBE_CFG_BRATE_SHIFT)

#define AMBE_CFG_RSVD2          (1 << 19)
#define AMBE_CFG_PARITYENAB     (1 << 20)
#define AMBE_CFG_RSVD3          (1 << 21)
#define AMBE_CFG_RSVD4          (1 << 22)
#define AMBE_CFG_RSVD5          (1 << 23)

typedef struct
{
	uint16_t length;
	uint8_t type;
	uint8_t data[64];

} AMBEPacketType;

typedef struct
{
	int16_t current;
	AMBEPacketType rx_packet[2];
	uint16_t tx_active;
	uint16_t tx_count;
	AMBEPacketType tx_packet;
	char version[50];
	uint16_t packets_sent;
	uint16_t packets_received;
} AMBEType;

extern AMBEType AMBE;
#define NO_PACKET (int16_t)-1

typedef enum
{
	ARM_LOAD_ERROR = 0,
	FI_LOAD_ERROR,
	AMBE_ERROR,
	RX_PACKET_ERROR,
	EEPROM_ERROR,
	ERROR_BUFFER_FULL,
	LAST_ERROR,
} ErrorType;

void Error(ErrorType error, uint16_t suberror);

void AMBE_UART_Init(void);
void AMBE_Packet_FSM(uint8_t byte);
void AMBE_Send(uint8_t* payload, uint16_t length, uint8_t type);
void AMBE_SendVoice(void);
void AMBE_SendPacket(void);

int AMBE_Reset(void);
void AMBE_Version(void);
void AMBE_Set_Rate(uint16_t rate);
void AMBE_Set_ECMode(uint16_t ecmode);
void AMBE_Start_Codec(void);
void AMBE_Stop_Codec(void);
void AMBE_Soft_Reset(void);
