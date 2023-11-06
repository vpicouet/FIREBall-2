/**
*************************************************************************
**	Include file for all 85230 ESCC register operations
**
**	Refer to Chapter 5 of the Zilog ESCC Users Manual
**	for register functionality details
**/


// ESCC register offset
#define REG0		0x00
#define REG1		0x01
#define REG2		0x02
#define REG3		0x03
#define REG4		0x04
#define REG5		0x05
#define REG6		0x06
#define REG7		0x07
#define REG8		0x08
#define REG9		0x09
#define REG10		0x0a
#define REG11		0x0b
#define REG12		0x0c
#define REG13		0x0d
#define REG14		0x0e
#define REG15		0x0f

#define ESCC_TX_FIFO 				4
#define COMSYNC_ESCC_IO_SIZE 			0x40
#define COMSYNC_ESCC_PORT_IO_OFFSET 	0x20
#define COMSYNC_ESCC_PORT_IO_SIZE 		0x10

#define CH_A 0 // channel A
#define CH_B 1 // channel B

// Comsync borad defined I/O address offsets
#define CH_A_DATA			0
#define CH_A_CONTROL		1
#define CH_B_DATA			2
#define CH_B_CONTROL		3
#define STATUS_CONTROL		4
#define TC_SECURITY			5
#define CONTROL_85230		6
#define MISC_CONTROL		7



// interrupt vector
// Transmit buffer empty
#define CH_A_TX		4
#define CH_B_TX		0
// External interrupt
#define CH_A_EXT	5
#define CH_B_EXT	1
// Received character
#define CH_A_RX		6
#define CH_B_RX		2

#define     RESET_EXT_STATUS_INT        0x10
#define     ENABLE_INT_NEXT_RX_CHAR     0x20
#define     RESET_TX_INT_PEND           0x28
#define     RESET_ERROR		            0x30
#define     RESET_HIGHEST_IUS           0x38
#define     RESET_RX_CRC                0x40
#define     RESET_TX_CRC                0x80
#define     RESET_TX_UR_EOM             0xC0

#define		CRC_SIZE					2



typedef enum {
	V11,
	EIA530A,
	EIA530,
	X21,
	V35,
	RS449,
	RS232
}_LIM;

typedef unsigned char byte;
/**
*************************************************************************
**	Write registers
**/
/*************************************************************************/
typedef volatile union {
	struct {
		byte reg_sel:3;
		byte cmd:3;
		byte crc_rst:2;
	} bf;
	byte reg;
} _wr0;
// Values for 'cmd' field
#define NULL_CODE						0
#define POINT_HIGH						1
#define RESET_EXT_STAT_INTERRUPTS		2
#define SEND_ABORT_ESCC					3
#define ENABLE_INT_ON_NEXT_RX			4
#define RESET_TX_INT_PENDING			5
#define ERROR_RESET						6
#define RESET_HIGHEST_IUS_CMD			7
// Values for 'crc_rst' field
#define RESET_RX_CRC_CHECKER			1
#define RESET_TX_CRC_CHECKER			2
#define RESET_TX_UNDERRUN_OEM_LATCH		3

#define  RX_AVAIL_ESCC				0x01
#define  ZERO_COUNT 				0x02
#define  DCD						0x08
#define  SYNC_HUNT					0x10
#define  CTS						0x20
#define  TX_UNDERRUN				0x40
#define  BREAK_ABORT				0x80

#define	 RESIDUE_0					0x08
#define	 RESIDUE_1					0x04
#define	 RESIDUE_2					0x02
#define	 ALL_SENT_ESCC				0x01
#define  PARITY_ERROR				0x10
#define  OVERRUN_ERROR				0x20
#define  FRAME_CRC_ERROR			0x40
#define  FRAME_END_SDLC				0x80






/*************************************************************************/
typedef volatile union {
	struct {
		byte ext_stat_mie:1;
		byte tx_int_enab:1;
		byte parity_special:1;
		byte rx_int_mode:2;
		byte wait_req:1;
		byte wait_dma:1;
		byte wait_dma_enab:1;
	} bf;
	byte reg;
} _wr1;
// Values for 'rx_int_mode' field
#define RX_INT_DISABLE							0
#define RX_INT_ON_FIRST_CH_OR_SPEC_CON		1
#define INT_ON_ALL_RX_CH_OR_SPEC_CON		2
#define RX_INT_ON_SPEC_CON_ONLY				3

/*************************************************************************/
typedef volatile struct {
	byte int_vector;
} _wr2;

/*************************************************************************/
typedef volatile union {
	struct {
		byte rx_enab:1;
		byte sync_load_inh:1;
		byte sdlc_search_mode:1;
		byte rx_crc_enab:1;
		byte enter_hunt:1;
		byte auto_enab:1;
		byte rx_bits:2;
	} bf;
	byte reg;
} _wr3;

// Values for 'rx_bits' field
#define RX_5			0
#define RX_6			1
#define RX_7			2
#define RX_8			3

/*************************************************************************/
typedef volatile union {
	struct {
		byte parity:2; // 00 none parity, 01 odd parity, 11 even parity
		byte mode:2;
		byte sync_mode:2;
		byte clock_mode:2;
	} bf;
	byte reg;
} _wr4;

#define PARITY_NONE 	0
#define PARITY_ODD 		1
#define PARITY_EVEN 	3

// Values for 'mode' field
#define SYNC_MODE_ENAB			0
#define ASYNC_1_STOP			1
#define ASYNC_15_STOP			2		// actually 1.5 stop bits
#define ASYNC_2_STOP			3
// Values for 'sync_mode' field
#define SYNC_8_MODE				0
#define SYNC_16_MODE			1
#define SDLC_MODE				2
#define EXT_SYNC_MODE			3
// Values for 'clock_mode' field
#define X1_CLK					0
#define X16_CLK					1
#define X32_CLK					2
#define X64_CLK					3

/*************************************************************************/
typedef volatile union {
	struct {
		byte tx_crc_enab:1;
		byte rts:1;
		byte poly_sel:1;
		byte tx_enab:1;
		byte tx_break:1;
		byte tx_bits:2;
		byte dtr:1;
	} bf;
	byte reg;
} _wr5;

// Values for 'tx_bits' field
#define TX_5			0
#define TX_6			1
#define TX_7			2
#define TX_8			3

/*************************************************************************/
typedef volatile struct {
	byte sync_sdlc_addr;
} _wr6;

/*************************************************************************/
typedef volatile struct {
	byte sync_sdlc_flag;	// when _wr15.select_wr7p=0
} _wr7;

/*************************************************************************/
typedef volatile union {
	struct {
		byte auto_tx_sdlc_flag:1;
		byte auto_eom_reset:1;
		byte auto_rts:1;
		byte rx_fifo_level:1;
		byte dtr_req_timing:1;
		byte tx_fifo_level:1;
		byte ext_read_enab:1;
		byte :1;			// unused bit
	} bf;					// when _wr15.select_wr7p=1
	byte reg;
} _wr7p;

/*************************************************************************/
typedef volatile struct {
	byte tx_data;
} _wr8;

/*************************************************************************/
typedef volatile union {
	struct {
		byte vector_incl_stat:1;
		byte no_vector_sel:1;
		byte disab_lower_chain:1;
		byte mie:1;
		byte status_high:1;
		byte soft_int_ack:1;
		byte device_reset:2;
	} bf;
	byte reg;
} _wr9;
// Values for 'device_reset' field
#define NO_RESET				0
#define RESET_B				1
#define RESET_A				2
#define HARD_RESET			3

/*************************************************************************/
typedef volatile union {
	struct {
		byte six8_sync_sel:1;
		byte loop_mode:1;
		byte abort_flag_on_underrun:1;
		byte mark_flag_idle:1;
		byte gaop:1;
		byte data_encoding:2;
		byte crc_preset:1;
	} bf;
	byte reg;
} _wr10;

/**
**	values for 'encoding'
**/
#define ENC_NRZ			0
#define ENC_NRZI		1
#define ENC_FM0			3
#define ENC_FM1			2
#define ENC_MANCH		0

/*************************************************************************/
typedef volatile union {
	struct {
		byte trxc_source:2;
		byte trxc_io:1;
		byte tx_clock:2;
		byte rx_clock:2;
		byte rtxc_xtal:1;
	} bf;
	byte reg;
} _wr11;

// Values for 'trxc_source' field
#define TRXC_XTAL				0
#define TRXC_TXCLK_ESCC			1
#define TRXC_BRG_ESCC			2
#define TRXC_DPLL_ESCC			3
// Values for 'tx_clock' field
#define TXCLK_RTXC_ESCC				0
#define TXCLK_TRXC_ESCC				1
#define TXCLK_BRG_ESCC				2
#define TXCLK_DPLL_ESCC				3
// Values for 'rx_clock' field
#define RXCLK_RTXC_ESCC				0
#define RXCLK_TRXC_ESCC				1
#define RXCLK_BRG_ESCC				2
#define RXCLK_DPLL_ESCC				3

/*************************************************************************/
typedef volatile struct {
	byte tc_low;
} _wr12, _rr12;

/*************************************************************************/
typedef volatile struct {
	byte tc_high;
} _wr13, _rr13;

/*************************************************************************/
// Values for 'dpll_cmd' field
typedef enum {
	NULL_CMD_ESCC,
	ENTER_SEARCH_MODE,
	RESET_MISSING_CLK,
	DISABLE_DPLL,
	SRC_BRG,
	SRC_RTXC,
	FM_MODE,
	NRZI_MODE
}_DPLL_CMD;

typedef volatile union {
	struct {
		byte brg_enab:1;
		byte brg_source:1;
		byte dtr_req:1;
		byte auto_echo:1;
		byte local_loopback:1;
		byte dpll_cmd:3;
	} bf;
	byte reg;
} _wr14;


/*************************************************************************/
typedef volatile union {
	struct {
		byte select_wr7p:1;				// reads as '0' on _rr15
		byte zero_count_int_enab:1;
		byte stat_fifo_enab:1;			// reads as '0' on _rr15
		byte dcd_int_enab:1;
		byte sync_hunt_int_enab:1;
		byte cts_int_enab:1;
		byte tx_underrun_int_enab:1;
		byte break_int_enab:1;
	} bf;
	byte reg;
} _wr15, _rr15;

/**
*************************************************************************
**	Read registers
**/
/*************************************************************************/
typedef volatile union {
	struct {
		byte rx_avail:1;
		byte zero_count:1;
		byte tx_buff_mty:1;
		byte dcd:1;
		byte sync_hunt_stat:1;
		byte cts:1;
		byte tx_underrun:1;
		byte rx_break:1;
	} bf;
	byte reg;
} _rr0;

/*************************************************************************/
typedef volatile union {
	struct {
		byte all_sent:1;
		byte residue_code:3;	// see Users manual for values, page 5-25
		byte parity_err:1;
		byte rx_overrun:1;
		byte crc_err:1;
		byte sdlc_eof:1;
	} bf;
	byte reg;
} _rr1;

/*************************************************************************/
typedef volatile struct {
	byte int_vector;
} _rr2;

/*************************************************************************/
typedef volatile struct {
	byte int_pending;
} _rr3;

/*************************************************************************/
typedef volatile union {
	byte wr4;		// when _wr7p.ext_read_enab=1
	byte rr0;		// else
} _rr4;

/*************************************************************************/
typedef volatile union {
	byte wr5;		// when _wr7p.ext_read_enab=1
	byte rr1;		// else
} _rr5;

/*************************************************************************/
typedef volatile union {
	byte sdlc_byte_count_low;			// when _wr15.stat_fifo_enab=1
	byte rr2;							// else
} _rr6;

/*************************************************************************/
typedef volatile union {
	struct {
		byte sdlc_byte_count_high:6;
		byte fifo_data_available:1;		// data book inconsistency !!!, see page 5-27
		byte fifo_overflow:1;			// data book inconsistency !!!
	} _sfe;								// when _wr15.stat_fifo_enab=1
	byte rr3;							// else
} _rr7;

/*************************************************************************/
typedef volatile struct {
	byte rx_data;
} _rr8;

/*************************************************************************/
typedef volatile union {
	byte wr3;			// when _wr7p.ext_read_enab=1
	byte rr13;			// else
} _rr9;

/*************************************************************************/
typedef volatile union {
	struct {
		byte :1;			//unused bit
		byte sdlc_onloop:1;
		byte :2;			//unused bits
		byte loop_sending:1;
		byte :1;			//unused bit
		byte two_clks_missing:1;
		byte one_clk_missing:1;
	} bf;
	byte reg;
} _rr10;

/*************************************************************************/
typedef volatile union {
	byte wr10;			// when _wr7p.ext_read_enab=1
	byte rr15;			// else
} _rr11;

/*************************************************************************/
// _rr12 and _rr13.... see _wr12 and _wr13

/*************************************************************************/
typedef volatile union {
	byte wr7p;				// when _wr7p.ext_read_enab=1
	byte rr10;				// else, !!data sheet inconsistency, might be rr14
} _rr14;

/*************************************************************************/
// _rr15.... see _wr15

/**
*************************************************************************
**	Wed Mar 20 15:32:36 EST 2002
**	The SCC itself
**/
typedef volatile struct {
	union {_wr0 wr0; _rr0 rr0;} r0;
	union {_wr1 wr1; _rr1 rr1;} r1;
	union {_wr2 wr2; _rr2 rr2;} r2;
	union {_wr3 wr3; _rr3 rr3;} r3;
	union {_wr4 wr4; _rr4 rr4;} r4;
	union {_wr5 wr5; _rr5 rr5;} r5;
	union {_wr6 wr6; _rr6 rr6;} r6;
	union {_wr7 wr7; _rr7 rr7;} r7;
	union {_wr8 wr8; _rr8 rr8;} r8;
	union {_wr9 wr9; _rr9 rr9;} r9;
	union {_wr10 wr10; _rr10 rr10;} r10;
	union {_wr11 wr11; _rr11 rr11;} r11;
	union {_wr12 wr12; _rr12 rr12;} r12;
	union {_wr13 wr13; _rr13 rr13;} r13;
	union {_wr14 wr14; _rr14 rr14;} r14;
	union {_wr15 wr15; _rr15 rr15;} r15;
	_wr7p r7p;		// MUST!!!! be located at the end of this structure
} _scc;

