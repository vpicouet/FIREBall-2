/*******************************************************************************
**	File:			cspci_pset.h
**	Author:			Rod Dore, Hasan Jamal
**
**
**	Description:
**		Port Settings include file
**
**	Copyright (c) 2009, CTI, Connect Tech Inc. All Rights Reserved.
**
**	THIS IS THE UNPUBLISHED PROPRIETARY SOURCE CODE OF CONNECT TECH INC.
**	The copyright notice above does not evidence any actual or intended
**	publication of such source code.
**
**	This module contains Proprietary Information of Connect Tech, Inc
**	and should be treated as Confidential.
******************************************************************************/


#ifndef _BHNS_pset_h
	#define _BHNS_pset_h



	//	Internal reference frequency (connected to Port-0 on IUSC chip)
	#define INT_REF_FREQ			18432000

	//	TX and RX Clocking Sources (Clock Tree, Level A)
	typedef enum {
		CLK_NONE	=	0,			//	No clock applied (turns off the respective Serial section)
		CLK_RXC		=	1,			//	From RxC external line interface signal
		CLK_TXC		=	2,			//	From TxC external line interface signal
		CLK_DPLL	=	3,			//	From DPLL
		CLK_BRG0	=	4,			//	From Baud rate generator 0
		CLK_BRG1	=	5,			//	From Baud rate generator 1
		CLK_CTR0	=	6,			//	from CTR0 (by-passes BRG)
		CLK_CTR1	=	7,			//	from CTR1 (by-passes BRG)
		CLK_INT_REF	=	0x86,		//	From Internal reference (18.432 MHz), IUSC Port-0 pin) (bypasses BRG and CTR)
		CLK_EXT_REF	=	0x87		//	From External reference (External reference, IUSC Port-1 pin) (bypasses BRG and CTR)
	} ct_levelA;

	//	DPLL clocking sources (Clock Tree, Level B)
	//	only used when Level-A selection is 'CLK_DPLL'
	typedef enum {
		DPLL_BRG0	=	0,		//	from BRG0
		DPLL_BRG1	=	1,		//	from BRG1
		DPLL_RXC	=	2,		//	from RxC external line interface signal
		DPLL_TXC	=	3		//	from TxC external line interface signal
	} ct_levelB;

	// BRG clocking sources (Clock Tree, Level C)
	//	Only used when Level-A selection is 'CLK_BRG0' or 'CLK_BRG1'
	//	or when Level-B selection is 'DPLL_BRG0' or 'DPLL_BRG1'.
	//	The BRG-# is selected by the Level-A setting
	typedef enum {
		BRG_CTR0		=	0,		//	from IUSC CTR-0 (or Port-0 if INT_REF is set)
		BRG_CTR1		=	1,		//	from IUSC CTR-1 (or Port-1 if EXT_REF is set)
		BRG_RXC			=	2,		//	from RxC external line interface signal
		BRG_TXC			=	3,		//	from TxC external line interface signal
		BRG_INT_REF		=	0x80,	//	CLKing directly from Port0 Pin (CTR-0 bypassed)
		BRG_EXT_REF		=	0x81	//	CLKing directly from Port1 Pin (CTR-1 bypassed)
									//	Note:	The bypass applies to both CTR's simultaneously !!
	} ct_levelC;

	// CTR (Prescaler) clocking sources (Clock Tree, Level D)
	typedef enum {
		CTR_DISABLE		=	0,		//	disable counter
		CTR_REF			=	1,		//	from their respective references CTR-0 is clocked from Port-0, CTR-1 is clocked from Port-1
		CTR_RXC			=	2,		//	from RxC external line interface signal
		CTR_TXC			=	3		//	from TxC external line interface signal
	} ct_levelD;

	//	TxC and RxC pin modes (and clocking sources)
	//		#defines that start with "XC_" apply to both the TxC and RxC pins
	//		#defines that start with "TXC_" apply only to the TxC pin
	//		#defines that start with "RXC_" apply only to the RxC pin
	#define	XC_INP			0		// input
	#define	TXC_TXCLK		1		// output, mirror the Transmitter clock (TxC pin only)
	#define RXC_RXCLK		1		// output, mirror the Receiver clock (RxC pin only)
	#define	XC_CHAR_CLK		2		// output, RxCHAR for RxC, TxCHAR for TxC
	#define	TXC_TXCOMP		3		// output, TX Complete (TxC pin only)
	#define	RXC_RXSYNC		3		// output, RxSYNC (RxC pin only)
	#define	XC_BRG0			4		// output, from BRG-0
	#define	XC_BRG1			5		// output, from BRG-1
	#define	TXC_CTR1		6		// output, from CTR-1 (TxC pin only)
	#define	RXC_CTR0		6		// output, from CTR-0 (RxC pin only)
	#define TXC_PORT1		0x86	// output, from Port-1 (TxC pin only)
	#define RXC_PORT0		0x86	// output, from Port-0 (RxC pin only)
	#define	XC_DPLL			7		// output, from DPLL-RX for RxC, DPLL-TX for TxC
	#define RXC_RXCLK_ESCC	8		// output, from ESCC RTxC pin

	// Serial Mode Selections
	#define SMODE_ASYNC		0		//	Asynchronous
	#define SMODE_EXT_SYNC	1		//	External Sync (Receiver only)
	#define SMODE_ISO			2		//	Isochronous,  ESCC does not support 
	#define SMODE_MONO		4		//	MonoSync
	#define SMODE_BISYNC		5		//	BiSync
	#define SMODE_HDLC		6		//	HDLC/SDLC
	#define SMODE_TRANS_BI	7		//	Transparent BiSync,  ESCC does not support 
	#define SMODE_9BIT		8		//	9-Bit Asynchronous,  ESCC does not support 

	// TX Encoding, RX Decoding Selections
	#define	NRZ				0
	#define	NRZ_INV			1
	#define	NRZI_MARK		2 // ESCC does not support 
	#define	NRZI_SPACE		3 // ESCC does not support 
	#define	BIPH_MARK		4
	#define	BIPH_SPACE		5
	#define	BIPH_LEVEL		6 // ESCC does not support 
	#define	BIPH_DIFF		7 // ESCC does not support 

	// Line Mode Selections
	//	See SP508 data sheet for details

	#define LM_530A			1		//	RS422 (with V.10 DTR)
	#define LM_530			2		//	RS422 (all signals V.11)
	#define LM_X21			3		//	Identical to LM_530 (except RI not available)
	#define LM_V35			4		//	V.35 on data/clocks, V.28 on all other signals
	#define LM_449			5		//	RS449 (Identical to LM_530 setting)
	#define LM_V28			6		//	RS232 (V.28)
	#define LM_SHUTDOWN		7		//	All Inputs/Output on SP508 are Tri-stated
	#define LM_TERM			0x08	//	Termination Enable, can be OR'd with above values

	//	Duplex Settings
	#define DUPLEX_FULL		0
	// follwing duplex settings not supported by ESCC
	#define DUPLEX_TX_COMP		0x10	// use the IUSC TX Complete signal to control
						// the line transceivers (else RTS will be used)
						// this value can be OR'd with the TWO_WIRE or FOUR_WIRE setting
	#define TWO_WIRE_RTS		1
	#define TWO_WIRE_TXCOMP		(TWO_WIRE_RTS | DUPLEX_TX_COMP)
	#define FOUR_WIRE_RTS		2
	#define FOUR_WIRE_TXCOMP	(FOUR_WIRE_RTS | DUPLEX_TX_COMP)

	// Parity Selections
	#define PAR_NONE		0
	#define	PAR_EVEN		1
	#define	PAR_ODD			2
	#define	PAR_SPACE		3
	#define	PAR_MARK		4

	//	CRC Types, Start values and Modes
	//	Types
	#define CRC_NONE		0
	#define	CCITT			1
	#define	CRC16			2
	#define	CRC32			3 // ESCC does not support 
	//	Start values 
	#define CRC_START_0		0		//	start with all zeros crc
	#define CRC_START_1		1		//	start with all ones crc
	//	Modes (ESCC does not support)
	#define CRC_TX_UNDERRUN		0x01	//	CRC sent when transmitter underruns
	#define CRC_TX_EOFM			0x02	//	CRC sent then end of frame, end of message) occurs
										//	the above 2 settings can be OR'd together
	// Read Register 1 of ESCC
	#define	 RESIDUE_0					0x08
	#define	 RESIDUE_1					0x04
	#define	 RESIDUE_2					0x02
	#define	 ALL_SENT_ESCC				0x01
	#define  PARITY_ERROR				0x10
	#define  OVERRUN_ERROR				0x20
	#define  FRAME_CRC_ERROR			0x40
	#define  FRAME_END_SDLC				0x80																				

	//	Transmitter Idle Patterns
	#define IDLE_DEF		0		//	Default idle activity for a given serial mode, ESCC does not support
	#define IDLE_ALT01		1		//	Alternating zero's and one's (encoded), ESCC does not support
	#define IDLE_ZEROS		2		//	Continuous zero's (encoded), ESCC does not support
	#define IDLE_ONES		3		//	Continuous ones' (encoded), ESCC does not support
	#define IDLE_MRK_SPC	5		//	Alternating Mark (1) and Space (0), ESCC does not support
	#define IDLE_SPACE		6		//	Continuous zero's (NOT encoded), ESCC does not support
	#define IDLE_MARK		7		//	Continuous ones's  (NOT encoded) ESCC supports this

	//	Transmitter Preamble Patterns, ESCC does not support
	#define PREAM_DISABLE	0		//	disabled
	#define PREAM_ZEROS		'0'		//	all zero's
	#define PREAM_ONES		'1'		//	all one's
	#define PREAM_FLAGS		'F'		//	flags (only in HDLC mode)
	#define PREAM_ZERO_ONE	'Z'		//	0101... pattern
	#define PREAM_ONE_ZERO	'O'		//	1010... pattern

//	HDLC Transmitter Underrun choices
	#define HDLC_UNDERRUN_ABORT_7		0x00	//	send 7 bit ABORT sequence (default)
	#define HDLC_UNDERRUN_ABORT_15		0x01	//	send 15 bit ABORT sequence,  ESCC does not support
	#define HDLC_UNDERRUN_FLAG			0x02	//	send FLAG

	//	HDLC Receiver "Address and Control Field" handling choices, ESCC does not support
	#define HDLC_RX_NO_ADDR_CONT		0
	#define HDLC_RX_ADDR1_CONT0			1
	#define HDLC_RX_ADDR1_CONT1			2
	#define HDLC_RX_ADDR1_CONT2			6
	#define HDLC_RX_EXADDR_CONT1		3
	#define HDLC_RX_EXADDR_CONT2		7
	#define HDLC_RX_EXADDR_CONT_GE2		11
	#define HDLC_RX_EXADDR_CONT_GE3		15

	#pragma pack(push,1)

	//=============================================================================================
	//	Sub-type for the data type below
	//
	typedef struct {
		unsigned long ref_freq;		//	Reference frequency source (frequency in Hz), use 'INT_REF_FREQ' for internal reference
		unsigned long bps_error;	//	Desired maximum bps error (in PPM, 10000 = 1%), can't always be achieved
		unsigned long bps;			//	Bit (baud) rate (integer portion)
		unsigned char bps_frac;		//	Bit (baud) rate (fractional portion), in 10th's, 5=.5 (only effective at low bit rates)
		unsigned char async_div;	//	16, 32 or 64 (needed for Async mode only)
		unsigned char dpll_div;		//	8, 16 or 32 (needed for receiver DPLL mode only)
		unsigned char ctr_div;		//	4, 8, 16 or 32 (only needed when the level-A clock tree setting is CTR0 or CTR1)
		unsigned char enc_dec;		//	See: TX Encoding, RX Decoding Selections above
		struct {
			ct_levelA A;
			ct_levelB B;
			ct_levelC C;
			ct_levelD D;
		} clk_tree;
		unsigned char clk_pin;		//	RxC or TxC Pin Mode
	} _trclk;

	//=============================================================================================
	// Data type defining how to setup a "Port"
	//
	typedef struct {
		//	port# (0 -> N-1) associated with the settings below
		char port;					//	this is here for convenience

		//	Serial mode
		//		The transmitter and receiver serial mode is usually the same, but the IUSC does not explicity enforce this.
		//		Although, there are combinations that won't operate properly.
		//	See: Serial Mode Selections above
		struct {
			unsigned char tx;		//	transmitter
			unsigned char rx;		//	receiver
		} smode;

		//	Receiver clocking selections
		//	See: Clock Tree settings above and above sub-structure
		_trclk rxclk;

		//	Transmitter clocking selections
		//	See: Clock Tree settings above and above sub-structure
		_trclk txclk;

		//	TX Preamble and Idle
		//	See: Transmitter Idles and Preamble Patterns above
		struct {
			unsigned char pre_pat;		//	preamble pattern
			unsigned char pre_len;		//	8,16,32,64 bits
			unsigned char tx_idle;		//	transmitter idle activity
		} tx_pre_idle;

		//	Electrical line interface mode plus termination
		//	See: Line Mode Selections above
		unsigned char line_mode;

		//	Duplex mode
		//	See: Duplex Settings above
		unsigned char duplex_mode;

		// Sync characters (or HDLC addresses)
		struct {
			unsigned char tx0;			//	SYN0 character (MonoSync & BiSync)
			unsigned char tx1;			//	SYN1 character (BiSync)
			unsigned char tx_len;		//	Number of bits in sync character(s) (8= always 8, 0=tx length)
			unsigned char rx0;			//	SYN0 character (MonoSync & BiSync, or first HDLC address byte)
			unsigned char rx1;			//	SYN1 character (BiSync, or second HDLC address byte)
			unsigned char rx_len;		//	Number of bits in sync character(s) (8= always 8, 0=rx length)
			unsigned char strip_sync;	//	!=0 enables this mode (applies to the Recevier only);// ESCC HDLC mode will enable address search for four MSB of rx0
		} sync_addr;

		//	Data bits (number of)
		struct {
			unsigned char tx;	//	1 to 8
			unsigned char rx;	//	1 to 8
		} dbits;

		//	Async Mode Transmitter Stop (fractional) bits
		//		units of 1/16th of a bit time, min=9, max=32 (16=1 stop bit)
		unsigned char tx_frac_stop;

		//	Parity selections
		//	See: Parity Selections above
		struct {
			unsigned char tx;
			unsigned char rx;
		} parity;

		//	CRC selections
		//	See: CRC Types and Settings above
		struct {
			unsigned char tx_type;
			unsigned char tx_start;
			unsigned char tx_mode;
			unsigned char rx_type;
			unsigned char rx_start;
		} crc;

		// ASYNC parameters (inherited from older BHN_PortSettings structure)
		//		with redundant items removed... baud, mode, bits, parity, stop
		struct  {

			unsigned char sflow;		//	Software (in-band) flow control modes;
										//		'n,r,t,b'
										//			'n'=none
										//			'r'=receive (Xoff/On transmitted when receiver fills/empties)
										//			't'=transmit (Xoff/On received will stop/start TX)
										//			'b'=both (receive and transmit)
			unsigned char xoff;					//	Xoff value
			unsigned char xon;					//	Xon value
			unsigned char hflow;		//	Hardware (outof-band) flow control modes;

		} async_settings;

		// New HDLC settings (started on Jan. 7, 2009)
		struct {
			unsigned char tx_flag_share_zero;	//	non-zero value will enable the shared ZERO bit between consequtive FLAGS (during IDLE)
			unsigned char rx_addr_control;		//	how the HDLC Receiver will handle Address and Control Fields (see 5.14.1 in manual)
			unsigned char tx_underrun;			//	Transmitter underrun behaviour (also see CRC setting CRC_TX_UNDERRUN)
		}  hdlc;

		unsigned char lloop;              //ESCC	Local Loopback; 0=Off, !=0 is On

		// flag to indicate write dma to disable
		char disable_dma_write;
		// flag to indicate read dma to disable
		char disable_dma_read;		
		//flag to enable CRC status padding as the first byte.
		unsigned char last_char_crc_stat;

	} BHN_PortSettings;


	#pragma pack(pop)

	#define FIX_TX_LOAD		0xFFff

		//	Return values (error codes) for function "iusc_port_setup"
		//		values are type "int"
		#define SUCCESS					0
		#define MSG_SUCCESS				"Setup was successful"

		// Primary errors
		#define GENERAL_ERROR			0x10000
		#define MSG_GENERAL_ERROR		"Error occured setting up features that support either the Receiver or Transmitter"
		#define TX_ERROR				0x20000
		#define MSG_TX_ERROR			"Error occured setting up the Transmitter section of the IUSC"
		#define RX_ERROR				0x40000
		#define MSG_RX_ERROR			"Error occured setting up the Receiver section of the IUSC"

		//	Error group
		#define PARAM_ERROR				0x1000	//	value of a settings stucture member is not valid
		#define MSG_PARAM_ERROR		"Value of a setup structure member (parameter) is not valid"
		#define INVALID_ERROR			0x2000	//	combinational or computational problem resulting from settings
		#define MSG_INVALID_ERROR	"An invalid combination of settings, or a computational problem (likely bit rate), has occured"
		#define CODE_ERROR				0x4000	//	Coding error (sub-function called with wrong value)
		#define MSG_CODE_ERROR		"Error in the code construction (call CTI)"

		// Errors that relate to parameter settings
		#define BAD_port				0x10
		#define MSG_BAD_port			"The port number is invalid"
		#define BAD_smode				0x20
		#define MSG_BAD_smode			"The Serial Mode setting is invalid"
		#define BAD_async_div			0x30
		#define MSG_BAD_async_div		"The Async divisor setting is invalid"
		#define BAD_dpll_div			0x40
		#define MSG_BAD_dpll_div		"The DPLL divisor setting is invalid"
		#define BAD_ctr_div				0x50
		#define MSG_BAD_ctr_div			"The CTR divisor setting is invalid"
		#define BAD_enc_dec				0x60
		#define MSG_BAD_enc_dec			"The data Encoding/Decoding setting is invalid"
		#define BAD_clk_treeA			0x70
		#define MSG_BAD_clk_treeA		"The Level-A clock source is invalid"
		#define BAD_clk_treeB			0x80
		#define MSG_BAD_clk_treeB		"The Level-B clock source is invalid"
		#define BAD_clk_treeC			0x90
		#define MSG_BAD_clk_treeC		"The Level-C clock source is invalid"
		#define BAD_clk_treeD			0xA0
		#define MSG_BAD_clk_treeD		"The Level-D clock source is invalid"
		#define BAD_clk_pin				0xB0
		#define MSG_BAD_clk_pin			"The clock pin selection is invalid"
		#define BAD_pre_pat				0xC0
		#define MSG_BAD_pre_pat			"The preamble pattern is invalid"
		#define BAD_pre_len				0xD0
		#define MSG_BAD_pre_len			"The preamble length is invalid"
		#define BAD_tx_idle				0xE0
		#define MSG_BAD_tx_idle			"The Line-IDLE setting is invalid"
		#define BAD_line_mode			0xF0
		#define MSG_BAD_line_mode		"The Line Mode setting is invalid"
		#define BAD_duplex_mode			0x100
		#define MSG_BAD_duplex_mode		"The Duplex Mode setting is invalid"
		#define BAD_sync_addr_len		0x110
		#define MSG_BAD_sync_addr_len	"The Sync character length setting in invalid"
		#define BAD_dbits				0x120
		#define MSG_BAD_dbits			"The number of Data bits is invalid"
		#define BAD_tx_frac_stop		0x130
		#define MSG_BAD_tx_frac_stop	"The Stop-bits setting is invalid (units of 1/16 bit)"
		#define BAD_parity				0x140
		#define MSG_BAD_parity			"The Parity setting is invalid"
		#define BAD_crc_type			0x150
		#define MSG_BAD_crc_type		"The CRC-type setting is invalid"
		#define BAD_crc_start			0x160
		#define MSG_BAD_crc_start		"The CRC-start setting is invalid"
		#define BAD_crc_mode			0x170
		#define MSG_BAD_crc_mode		"The CRC-mode setting is invalid"
		#define BAD_tx_trig_gate		0x180
		#define MSG_BAD_tx_trig_gate	"The Triggered/Gated Transmitter setting is invalid"
		#define BAD_rx_gate				0x190
		#define MSG_BAD_rx_gate			"The Gated Receiver setting is invalid"
		#define BAD_hdlc_rx_addr_cont	0x1A0
		#define MSG_BAD_hdlc_rx_addr_cont	"The Receiver 'Address/Control Field Handling' setting is invalid"
		#define BAD_hflow				0x1B0
		#define MSG_BAD_hflow			"The Hardware flow control setting is invalid"
		#define BAD_tx_underrun			0x1C0
		#define MSG_BAD_tx_underrun		"The TX Underrun setting is invalid"

		// Errors that related to the interaction of settings
		#define BRG_computation			0x200	//	The Bit Rate cannot be achieved with the settings and clocking combinations supplied
		#define MSG_BRG_computation		"The desired Bit-rate cannot be achieved with the frequency settings and/or clocking combinations specified"
		#define BRG_overload			0x210	//	use of a BRG is inconsistent between the Receiver, Transmitter, RxC-pin or TxC-pin
		#define MSG_BRG_overload		"A baud-rate generator is being used more than once, with different resulting divisor settings"
		#define CTR_overload			0x220	//	use of a CTR is inconsistent between the Receiver, Transmitter, RxC-pin or TxC-pin
		#define MSG_CTR_overload		"A counter is being used more than once, with different divisor setting and/or source selection"
		#define CIRCULAR_clocking		0x230	//	clocking selections have produced a "circular clocking" tree, which won't operate
		#define MSG_CIRCULAR_clocking	"A circular clocking situation exists (a clock output feeds its own input)"
		#define DPLL_inconsistent		0x240	//	Use of the DPLL is inconsistent between the Receiver, Transmitter, RxC-pin or TxC-pin
		#define MSG_DPLL_inconsistent	"Use of the DPLL (or its clock outputs) is inconsistent between the Receiver / Transmitter"
		#define HFLOW_DUPLEX_incompatible		0x250	//	Hardware flow control enabled and non-Full Duplex settings applied together
		#define MSG_HFLOW_DUPLEX_incompatible	"The use of Hardware flow control in any mode except Full-Duplex is invalid"
		#define HFLOW_TXTRIG_incompatible		0x260	//	Hardware flow control enabled and Triigered/Gated Transmission applied together
		#define MSG_HFLOW_TXTRIG_incompatible	"The use of Hardware flow control with Triggered/Gated Transmission is invalid"


	//=============================================================================================
	//	This setup mimics an ASYNC serial port, operting at 9600 baud
	//		and is used as the setup for operation of SCM
	//
	#define BHN_PORT_SETTINGS_DEFAULT(pnum)	\
		{	pnum,		/* Port number */			\
			{	/* Serial mode */					\
				SMODE_ASYNC,		/* tx */		\
				SMODE_ASYNC,		/* rx */		\
			},	\
			{	/* Receiver clocking selections */						\
				INT_REF_FREQ,	/* Internal 18.432 MHz reference */		\
				10000,			/* 1% desired bps accuracy */			\
				9600,			/* 9600.0 bps */						\
				0,														\
				16,				/* Async x16 clocking */				\
				0,				/* not required for Async mode */		\
				0,				/* not required for Async mode */		\
				NRZ,			/* data decoding */						\
				{														\
					CLK_BRG0,	/* clocked from BRG-0 */				\
					0,			/* not required */						\
					BRG_CTR0,	/* BRG-0 clocked from CTR-0 */			\
					CTR_REF,	/* CTR-0 clocked from Port-0 */			\
				},														\
				XC_INP,			/* RxC pin is an input */				\
			},	\
			{	/* Transmitter clocking selections */			\
				INT_REF_FREQ,									\
				10000,											\
				9600,											\
				0,												\
				16,												\
				0,		/* not required for Async mode */		\
				0,		/* not required for Async mode */		\
				NRZ,	/* data encoding */						\
				{	/* Clock tree*/								\
					CLK_BRG0,	/* clocked from BRG-0 */		\
					0,			/* not required */				\
					BRG_CTR0,	/* BRG0 clocked from CTR-0 */	\
					CTR_REF,	/* CTR-0 clocked from Port-0 */	\
				},												\
				XC_INP,			/* TxC pin is an input */		\
			},	\
			{	/* TX Preamble and Idle */						\
				0,			/* not required for Async mode */	\
				0,			/* not required for Async mode */	\
				IDLE_DEF,										\
			},	\
			/* Electrical line interface mode plus termination */				\
			LM_SHUTDOWN,	/* this is the power-up state of the SP508 */		\
			/* Duplex mode */		\
			DUPLEX_FULL,			\
			{	/* Sync characters (or HDLC addresses) */						\
				0,0,0,0,0,0,	/* no SYNC characters needed for Async mode */	\
			},	\
					\
			{	/* Data bits (number of) */		\
				8,		/* tx */				\
				8,		/* rx */				\
			},	\
			/* Async Mode Transmitter (fractional) Stop bits */		\
			16,		\
			{	/* Parity selections */		\
				PAR_NONE,	/* tx */		\
				PAR_NONE,	/* rx */		\
			},	\
			{	/* CRC selections */									\
				0,0,0,0,0,	/* not required for Async mode */		\
			},	\
			{	/* ASYNC Settings */	\
				'n', 0x13, 0x11, 0	\
			}	\
		}

	//=============================================================================================
	//	This setup is a Simple MonoSync setup:
	//		The TX is clocked internally by a BRG, and generates an external data clock output (TxC)
	//		The RX uses an external data clock input (RxC)
	//		NRZ data
	//		CRC generation and checking is enabled
	//		Tranmitter sends continuous MARKing when idle
	//		RS232 Line Interface
	//			Loopback: TX->RX, TXC->RXC
	//
	#define SIMPLE_MONOSYNC(pnum)	\
		{	\
			/* Port number */	\
			pnum,	\
\
			/* Serial mode */	\
			{	\
				SMODE_MONO,		/* tx */		\
				SMODE_MONO,		/* rx */		\
			},	\
\
			/* Receiver clocking selections */	\
			{	\
				0,			/* ref_freq not needed */		\
				0,			/* bps_error not needed */		\
				0,			/* bps not needed */			\
				0,			/* bps_frac not needed */		\
				0,			/* async_div not needed */		\
				0,			/* dpll_div not needed */		\
				0,			/* ctr_div not needed */		\
				NRZ,		/* data decoding */						\
				{												\
					CLK_RXC,	/* clocked from RXC pin */		\
					0,			/* not required */				\
					0,			/* not needed */				\
					0,			/* not needed */				\
				},														\
				XC_INP,			/* RxC pin is an input */				\
			},	\
\
			/* Transmitter clocking selections */	\
			{	\
				INT_REF_FREQ,									\
				10000,											\
				57600,											\
				0,		/* bps_frac not needed */				\
				0,		/* async_div not needed */				\
				0,		/* dpll_div not needed */				\
				0,		/* ctr_div not needed */				\
				NRZ,	/* data encoding */						\
				{	/* Clock tree*/								\
					CLK_BRG0,	/* clocked from BRG-0 */		\
					0,			/* not required */				\
					BRG_CTR0,	/* BRG0 clocked from CTR-0 */	\
					CTR_REF,	/* CTR-0 clocked from Port-0 */	\
				},												\
				TXC_TXCLK,		/* TxC pin is an output, from the Transmitter clocking source */		\
			},	\
\
			/* TX Preamble and Idle */						\
			{	\
				0,			/* not required */	\
				0,			/* not required */	\
				IDLE_MARK,	/* Continuous "mark" */				\
			},	\
\
			/* Electrical line interface mode plus termination */				\
			LM_SHUTDOWN,		/* Tri-state */		\
\
			/* Duplex mode */	\
			DUPLEX_FULL,		\
\
			/* Sync characters (or HDLC addresses) */	\
			{	\
				0x16,0,8,	/* TX SYN (MonoSync), 8 bits */		\
				0x16,0,8,	/* RX SYNC (mono), 8 bits */		\
				0,			/* don't strip SYN chars */		\
			},	\
\
			/* Data bits (number of) */		\
			{	\
				8,	/* tx */	\
				8,	/* rx */	\
			},	\
\
			/* Async Mode Transmitter (fractional) Stop bits */		\
			0,	\
\
			/* Parity selections */		\
			{	\
				PAR_NONE,	/* tx */	\
				PAR_NONE,	/* rx */	\
			},	\
\
			/* CRC selections */	\
			{	\
				CRC16, CRC_START_0, CRC_TX_UNDERRUN,	/* TX CRC setup	*/	\
				CRC16, CRC_START_0,		/* RX CRC setup */				\
			},	\
\
			/* ASYNC Settings */	\
			{	\
				'n', 0x13, 0x11, 0	\
			},	\
		}

	//=============================================================================================
	//	This setup is for externally clocked ASYNC data (mainly used to do a functional test of the Line I/F circuits)
	//		The TX is clocked internally by a BRG, and generates an external data clock output (TxC) which is 16X the bit rate
	//		The RX uses an external data clock input (RxC), which directly clocks the receiver (at 16X the data rate).
	//		NRZ data
	//		RS232 Line Interface
	//			Loopback: TX->RX, TXC->RXC
	//
	#define EXTERNALLY_CLOCKED_ASYNC(pnum)	\
		{	\
			/* Port number */	\
			pnum,	\
\
			/* Serial mode */	\
			{	\
				SMODE_ASYNC,		/* tx */		\
				SMODE_ASYNC,		/* rx */		\
			},	\
\
			/* Receiver clocking selections */	\
			{	\
				0,			/* ref_freq not needed */		\
				0,			/* bps_error not needed */		\
				0,			/* bps not needed */			\
				0,			/* bps_frac not needed */		\
				16,			/* async_div not needed */		\
				0,			/* dpll_div not needed */		\
				0,			/* ctr_div not needed */		\
				NRZ,		/* data decoding */						\
				{												\
					CLK_RXC,	/* clocked from RXC pin */		\
					0,			/* not required */				\
					0,			/* not needed */				\
					0,			/* not needed */				\
				},														\
				XC_INP,			/* RxC pin is an input */				\
			},	\
\
			/* Transmitter clocking selections */	\
			{	\
				INT_REF_FREQ,									\
				10000,											\
				57600,											\
				0,		/* bps_frac not needed */				\
				16,		/* async_div not needed */				\
				0,		/* dpll_div not needed */				\
				0,		/* ctr_div not needed */				\
				NRZ,	/* data encoding */						\
				{	/* Clock tree*/								\
					CLK_BRG0,	/* clocked from BRG-0 */		\
					0,			/* not required */				\
					BRG_CTR0,	/* BRG0 clocked from CTR-0 */	\
					CTR_REF,	/* CTR-0 clocked from Port-0 */	\
				},												\
				XC_BRG0,		/* TxC pin is an output, from BRG-0 */		\
			},	\
\
			/* TX Preamble and Idle */						\
			{	\
				0,			/* not required */	\
				0,			/* not required */	\
				IDLE_DEF,	/* default idle */				\
			},	\
\
			/* Electrical line interface mode plus termination */				\
			LM_V28,		/* RS232 */		\
\
			/* Duplex mode */	\
			DUPLEX_FULL,		\
\
			/* Sync characters (or HDLC addresses) */	\
			{	\
				0x16,0,8,	/* TX SYN (MonoSync), 8 bits */		\
				0x16,0,8,	/* RX SYNC (mono), 8 bits */		\
				0,			/* don't strip SYN chars */		\
			},	\
\
			/* Data bits (number of) */		\
			{	\
				8,	/* tx */	\
				8,	/* rx */	\
			},	\
\
			/* Async Mode Transmitter (fractional) Stop bits */		\
			0,	\
\
			/* Parity selections */		\
			{	\
				PAR_NONE,	/* tx */	\
				PAR_NONE,	/* rx */	\
			},	\
\
			/* CRC selections */	\
			{	\
				CRC16, CRC_START_0, CRC_TX_UNDERRUN,	/* TX CRC setup	*/	\
				CRC16, CRC_START_0,		/* RX CRC setup */				\
			},	\
\
			/* ASYNC Settings */	\
			{	\
				'n', 0x13, 0x11, 0	\
			},	\
		}

/*
**************************Used on the half duplex mode test **************************
*/
	#define CS_PCI_PORT_SETTINGS_ASYNC(pnum)	\
		{	pnum,		/* Port number */			\
			{	/* Serial mode */					\
				SMODE_ASYNC,		/* tx */		\
				SMODE_ASYNC,		/* rx */		\
			},	\
			{	/* Receiver clocking selections */						\
				INT_REF_FREQ,	/* Internal 18.432 MHz reference */		\
				10000,			/* 1% desired bps accuracy */			\
				115200,			/* 115200.0 bps */						\
				0,														\
				16,				/* Async x16 clocking */				\
				0,				/* not required for Async mode */		\
				0,				/* not required for Async mode */		\
				NRZ,			/* data decoding */						\
				{														\
					CLK_BRG0,	/* clocked from BRG-0 */				\
					0,			/* not required */						\
					BRG_CTR0,	/* BRG-0 clocked from CTR-0 */			\
					CTR_REF,	/* CTR-0 clocked from Port-0 */			\
				},														\
				XC_INP,			/* RxC pin is an input */				\
			},	\
			{	/* Transmitter clocking selections */			\
				INT_REF_FREQ,									\
				10000,											\
				115200,											\
				0,												\
				16,												\
				0,		/* not required for Async mode */		\
				0,		/* not required for Async mode */		\
				NRZ,	/* data encoding */						\
				{	/* Clock tree*/								\
					CLK_BRG0,	/* clocked from BRG-0 */		\
					0,			/* not required */				\
					BRG_CTR0,	/* BRG0 clocked from CTR-0 */	\
					CTR_REF,	/* CTR-0 clocked from Port-0 */	\
				},												\
				XC_INP,			/* TxC pin is an input */		\
			},	\
			{	/* TX Preamble and Idle */						\
				0,			/* not required for Async mode */	\
				0,			/* not required for Async mode */	\
				IDLE_DEF,										\
			},	\
			/* Electrical line interface mode plus termination */				\
			LM_530,	/* any V.11 mode would work */		\
			/* Duplex mode */		\
			DUPLEX_FULL,			\
			{	/* Sync characters (or HDLC addresses) */						\
				0,0,0,0,0,0,	/* no SYNC characters needed for Async mode */	\
			},	\
					\
			{	/* Data bits (number of) */		\
				8,		/* tx */				\
				8,		/* rx */				\
			},	\
			/* Async Mode Transmitter (fractional) Stop bits, 16=1 stop bit */		\
			16,		\
			{	/* Parity selections */		\
				PAR_NONE,	/* tx */		\
				PAR_NONE,	/* rx */		\
			},	\
			{	/* CRC selections */									\
				0,0,0,0,0,	/* not required for Async mode */		\
			},	\
			{	/* ASYNC Settings */	\
				'n', 0x13, 0x11, 0,	\
			}	\
		}

#endif
