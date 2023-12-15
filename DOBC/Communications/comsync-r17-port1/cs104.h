/**
*************************************************************************
**	Tue Mar 19 12:41:53 EST 2002
**	The I/O of the ComSync/104 card
**/
#define CS104_Adata			0
#define CS104_Actrl			1
#define CS104_Bdata			2
#define CS104_Bctrl			3
#define CS104_CtrlStat		4
#define CS104_TC_Secur		5
#define CS104_RST_IACK		6
#define CS104_Misc			7
#define CS104_IRQ			8
#define CS104_Aregs			0x20
#define CS104_Bregs			0x30

typedef struct {
	byte Adata;						// 0: Channel A data
	byte Actrl;						// 1: Channel A control
	byte Bdata;						// 2: CHannel B data
	byte Bctrl;						// 3: CHannel B data
	union {
		struct {
			byte DtrA:1;			// DTR, channel A
			byte DtrB:1;			// DTR, channel B
			byte Secur_enab:1;		// Security feature enable
			byte irq_sel:3;			// IRQ selection (on PC104 bus)
			byte DMA_enB:1;			// DMA enable, channel B
			byte DMA_enA:1;			// DMA enable, channel A
		} _ctrl;					// Write only
		struct {
			byte DsrA:1;			// DSR, channel A
			byte DsrB:1;			// DSR, channel B
			byte Secur_enab:1;		// Security feature enable
			byte :1;				// unused bit
			byte scc_int:1;			// SCC interrupt (active low)
			byte tc_int:1;			// TC interrupt (active high)
			byte DMA_enB:1;			// DMA enable, channel B
			byte DMA_enA:1;			// DMA enable, channel A
		} _stat;					// Read only
		byte reg;
	} _cs;							// 4: Control & Status
	union {
		byte clear_tc;				// Clear TC interrupt (Write any value)
		byte Read_SCode;			// Read Security Code
	} _m1;							// 5: Misc.1 I/O
	union {
		byte scc_intack;			// Read: Interrupt acknowledge cycle to SCC
		byte rst_scc;				// Write: Reset SCC (any value works)
	} _m2;							// 6: Misc.2 I/O
	union {
		struct {
			byte LimA:3;				// Line Interface mode, channel A
			byte LimB:3;				// Line Interface mode, channel B
			byte mie:1;					// Master interrupt enable
			byte wait_en:1;				// WAIT mode enable
		} bf;							// 7: Misc.3 I/O
		byte reg;
	} _m3;
	byte unused [24];		// needed for offsetof() calculations
	// The following are only valid if Enhanced I/O addressing is enabled
	_scc SccA;
	_scc SccB;
} _cs104;

/**
*************************************************************************
**	Thu Apr 25 11:15:57 EDT 2002
**	Comm modes and settings
**	used to setup settings ('scctty')
**/
typedef struct {
	unsigned long bps;
	unsigned char chan_number;
	unsigned char comm_mode;
	unsigned char encoding;
	unsigned char lim;
	unsigned char sync1;
	unsigned char sync2;
} _scc_setting;



/**
**	value for 'lim' (line interface mode)
**/
#define LIM_422			1
#define LIM_530A		2
#define LIM_530			3
#define LIM_X21			4
#define LIM_V35			5
#define LIM_449			6
#define LIM_232			7
#define MAX_LIM_SETTING	7

