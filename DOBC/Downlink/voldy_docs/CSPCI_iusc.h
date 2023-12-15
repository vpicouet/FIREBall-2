/*******************************************************************************
**	File:			CSPCI_iusc.h
**	Author:			Rod Dore, Hasan Jamal
**	
**
**	Description:
**		IUSC related declarions
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

#ifndef _CSPCI_iusc_h
	#define _CSPCI_iusc_h

	#include "CSPCI_hw.h"

	/****************************************************************************************/
	// Burst Dwell Control Register (BDCR)
	#define	MAX_XFERS		FIELD_MASK(8,8)
		#define SET_MAX_XFERS(n)	(((n) & 0xFF) << 8)
	#define	MAX_CLKS		FIELD_MASK(8,0)
		#define SET_MAX_CLKS(n)		((n) & 0xFF)

	/****************************************************************************************/
	// Channel Command/Address Register (CCAR)
	#define	RTCMD		FIELD_MASK(5,11)
		#define	RST_HIGHEST_SERIAL_IUS	(0x2 << 11)
		#define	TRIG_CHANNEL_LOAD_DMA	(0x4 << 11)
		#define	TRIG_RX_DMA				(0x5 << 11)
		#define TRIG_TX_DMA				(0x6 << 11)
		#define TRIG_RXTX_DMA			(0x7 << 11)
		#define	PURGE_RX_FIFO			(0x9 << 11)
		#define PURGE_TX_FIFO			(0xA << 11)
		#define PURGE_RXTX_FIFO			(0xB << 11)
		#define LOAD_RCC				(0xD << 11)
		#define	LOAD_TCC				(0xE << 11)
		#define	LOAD_RCCTCC				(0xF << 11)
		#define	LOAD_TC0				(0x11 << 11)
		#define	LOAD_TC1				(0x12 << 11)
		#define	LOAD_TC0TC1				(0x13 << 11)
		#define	DATA_LSB_FIRST			(0x14 << 11)
		#define	DATA_MSB_FIRST			(0x15 << 11)
		#define	D15_8_FIRST				(0x16 << 11)
		#define	D7_0_FIRST				(0x17 << 11)
		#define	PURGE_RX				(0x19 << 11)
	#define	RTRESET		FIELD_MASK(1,10)
	#define	RTMODE		FIELD_MASK(2,8)		//	set page 4-11 of IUSC manual
		#define	NORMAL_MODE				(0 << 8)
		#define	ECHO_RX2TX				(1 << 8)
		#define	LOCAL_LOOP				(2 << 8)
		#define	INTERNAL_LOCAL			(3 << 8)
	#define	CHANLOAD	FIELD_MASK(1,7)
	// The following definitions also apply to the DCAR register
	#define B_W			FIELD_MASK(1,6)
		#define	BYTE_ACC				(1 << 6)
	#define REGADDR		FIELD_MASK(5,1)				
	#define	U_L			FIELD_MASK(1,0)

	
	/****************************************************************************************/
	// Channel Command/Status Register (CCSR)
	#define	RCCF_OVFLO		FIELD_MASK(1,15)
	#define	RCCF_AVAIL		FIELD_MASK(1,14)
	#define	RCCF_CLEAR		FIELD_MASK(1,13)
	#define	DPLL_SYNC		FIELD_MASK(1,12)
	#define	DPLL_2MISS		FIELD_MASK(1,11)
	#define	DPLL_1MISS		FIELD_MASK(1,10)
	#define	DPLL_EDGE		FIELD_MASK(2,8)
		#define	DPLL_RF			(0 << 8)
		#define	DPLL_RISING		(1 << 8)
		#define	DPLL_FALLING	(2 << 8)
		#define	DPLL_FREE		(3 << 8)
	#define	ON_LOOP			FIELD_MASK(1,7)
	#define	LOOP_SEND		FIELD_MASK(1,6)
	#define	CTR01_BYPASS	FIELD_MASK(1,5)
	#define	TX_RESIDUE		FIELD_MASK(3,2)

	/****************************************************************************************/
	// Channel Control Register (CCR)
	//	All bits Read/Write
	#define TXCTRBLK			FIELD_MASK(2,14)
		#define NO_TCBS				(0 << 14)
		#define TX_TCB_32			(2 << 14)
	#define WAIT4_TXTRIG		FIELD_MASK(1,13)
		#define HOLD_TX_DMA_REQ		(1 << 13)
	#define FLAG_PREAMBLE		FIELD_MASK(1,12)	// only when PREAMBLE_ONES_FLAGS is used
		#define SEND_FLAGS_AS_PREAMBLE	(1 << 12)
	#define TXSHAVEL			FIELD_MASK(4,8)		//ASYNC mode
		#define SET_TXSHAVE(shave)				(((shave) & 0x0F) << 8)
	#define TXPREL				FIELD_MASK(2,10)	//Any SYNC mode
		#define PREAMBLE_8			(0 << 10)
		#define PREAMBLE_16			(1 << 10)
		#define PREAMBLE_32			(2 << 10)
		#define PREAMBLE_64			(3 << 10)
	#define TXPREPAT			FIELD_MASK(2,8)		//Any SYNC mode
		#define PREAMBLE_ZEROS			(0 << 8)
		#define PREAMBLE_ONES_FLAGS		(1 << 8)
		#define PREAMBLE_10				(2 << 8)
		#define PREAMBLE_01				(3 << 8)
	#define RXSTATBLK			FIELD_MASK(2,6)
		#define NO_RSBS				(0 << 6)
		#define RX_RSB_16			(1 << 6)
		#define RX_RSB_32			(2 << 6)
	#define WAIT4_RXTRIG		FIELD)MASK(1,5)
		#define HOLD_RX_DMA_REQ		(1 << 5)
	
	/****************************************************************************************/
	// Channel Mode Register (CMR)
	//	All bits Read/Write
	#define	TX_SUB_MODE		FIELD_MASK(4,12)
	#define	TX_MODE			FIELD_MASK(4,8)
	#define TX_MODE_SET(value)	((value) << 8)
		#define	TX_ASYNC		(SMODE_ASYNC << 8)
			#define	TX_1_STOP	(0 << 14)
			#define	TX_2_STOP	(1 << 14)
			#define	TX_1_SHAVED	(2 << 14)
			#define	TX_2_SHAVED	(3 << 14)
			#define	TX_X16		(0 << 12)
			#define	TX_X32		(1 << 12)
			#define	TX_X64		(2 << 12)
		#define	TX_ISO			(SMODE_ISO << 8)
			#define	TX_ISO_1_STOP	(0 << 14)
			#define	TX_ISO_2_STOP	(1 << 14)
		#define	TX_MONO			(SMODE_MONO << 8)
			#define	CRC_UNDERUN		(1 << 15)
			#define	PREAMBLE		(1 << 13)
			#define	TX_SYNCS_8		(0 << 12)
			#define	TX_SYNCS_TXLEN	(1 << 12)
		#define	TX_BI			(SMODE_BISYNC << 8)
			#define	CLOSE_SYN_TSR_HIGH	(0 << 14)
			#define	CLOSE_SYN01_TSR		(1 << 14)
		#define	TX_HDLC			(SMODE_HDLC << 8)
			#define	ABORT_7			(0 << 14)
			#define	ABORT_15		(1 << 14)
			#define	ABORT_FLAG		(2 << 14)
			#define	CRC_FLAG		(3 << 14)
			#define	FLAGS_SHARE_0	(1 << 12)
		#define	TX_TRANS_BI		(SMODE_TRANS_BI << 8)
			#define	CLOSE_SYN		(0 << 14)
			#define	CLOSE_DLE_SYN	(1 << 14)
			#define	SEND_ASCII		(0 << 12)
			#define	SEND_EBCDIC		(1 << 12)
		#define	TX_9BIT			(SMODE_9BIT << 8)
			#define	SEND_9_0		(0 << 15)
			#define	SEND_9_1		(1 << 15)
			#define	SEND_DATA_8		(0 << 14)
			#define	SEND_DATA_7P	(1 << 14)
		#define	TX_ETHER		(SMODE_ETHER << 8)
		#define	TX_SLAV_MONO	(TX_MONO | (1 << 11))
			#define	STOP_AT_EOM		(0 << 13)
			#define	ANOTHER_MSG		(1 << 13)
		#define	TX_HDLC_LOOP	(TX_HDLC | (1 << 11))
			#define	DIS_REPEAT		(0 << 13)
			#define	INSERT_SEND		(1 << 13)
	#define	RX_SUB_MODE		FIELD_MASK(4,4)
	#define	RX_MODE			FIELD_MASK(4,0)
	#define RX_MODE_SET(value)	((value) << 0)
		#define	RX_ASYNC		(SMODE_ASYNC << 0)
			#define	RX_X16			(0 << 4)
			#define	RX_X32			(1 << 4)
			#define	RX_X64			(2 << 4)
		#define	RX_EXT_SYNC		(SMODE_EXT_SYNC << 0)
		#define	RX_ISO			(SMODE_ISO << 0)
		#define	RX_MONO			(SMODE_MONO << 0)
			#define	STRIP_SYNC			(1 << 5)
			#define	INCLUDE_SYNC		(0 << 5)
			#define	RX_SYNCS_8			(0 << 4)
			#define	RX_SYNCS_RXLEN		(1 << 4)
		#define	RX_BI			(SMODE_BISYNC << 0)
		#define	RX_HDLC			(SMODE_HDLC << 0)
			#define	NO_ADDR_CONT		(0 << 4)
			#define	ADDR1_CONT0			(1 << 4)
			#define	ADDR1_CONT1			(2 << 4)
			#define	ADDR1_CONT2			(6 << 4)
			#define	EXADDR_CONT1		(3 << 4)
			#define	EXADDR_CONT2		(7 << 4)
			#define	EXADDR_CONT_GE2		(11 << 4)
			#define	EXADDR_CONT_GE3		(15 << 4)
		#define	RX_TRANS_BI		(SMODE_TRANS_BI << 0)
			#define	LOOK_ASCII		(0 << 4)
			#define	LOOK_EBCDIC		(1 << 4)
		#define	RX_9BIT			(SMODE_9BIT << 0)
		#define	RX_ETHER		(SMODE_ETHER << 0)
			#define	RX_ALL				(0 << 4)
			#define	MATCH16_DEST_ADDR	(1 << 4)

	/****************************************************************************************/
	// Clear DMA Interrupt Register (CDIR)
	#define CLR_RXDMA_IUS		FIELD_MASK(1,9)
	#define CLR_TXDMA_IUS		FIELD_MASK(1,8)
	#define CLR_RXDMA_IP		FIELD_MASK(1,1)
	#define CLR_TXDMA_IP		FIELD_MASK(1,0)
		#define CLR_ALL_CDIR	(CLR_RXDMA_IUS | CLR_TXDMA_IUS | CLR_RXDMA_IP | CLR_TXDMA_IP)
	
	/****************************************************************************************/
	// Clock Mode Control Register (CMCR)
	//	All bit Read/Write
	#define	CTR1_SRC		FIELD_MASK(2,14)
		#define	CTR1_DISABLE	(CTR_DISABLE << 14)
		#define	CTR1_PORT1		(CTR_REF << 14)
		#define	CTR1_RXC		(CTR_RXC << 14)
		#define	CTR1_TXC		(CTR_TXC << 14)
	#define	CTR0_SRC		FIELD_MASK(2,12)
		#define	CTR0_DISABLE	(CTR_DISABLE << 12)
		#define	CTR0_PORT0		(CTR_REF << 12)
		#define	CTR0_RXC		(CTR_RXC << 12)
		#define	CTR0_TXC		(CTR_TXC << 12)
	#define	BRG1_SRC		FIELD_MASK(2,10)
		#define	BRG1_CTR0_PORT0		(BRG_CTR0 << 10)
		#define	BRG1_CTR1_PORT1		(BRG_CTR1 << 10)
		#define	BRG1_RXC			(BRG_RXC << 10)
		#define	BRG1_TXC			(BRG_TXC << 10)
		#define BRG1_SRC_SET(value)		(((value) & 0x03) << 10)
	#define	BRG0_SRC		FIELD_MASK(2,8)
		#define	BRG0_CTR0_PORT0		(BRG_CTR0 << 8)
		#define	BRG0_CTR1_PORT1		(BRG_CTR1 << 8)
		#define	BRG0_RXC			(BRG_RXC << 8)
		#define	BRG0_TXC			(BRG_TXC << 8)
		#define BRG0_SRC_SET(value)		(((value) & 0x03) << 8)
	#define	DPLL_SRC		FIELD_MASK(2,6)
		#define	DPLL_SRC_BRG0		(DPLL_BRG0 << 6)
		#define	DPLL_SRC_BRG1		(DPLL_BRG1 << 6)
		#define	DPLL_SRC_RXC		(DPLL_RXC << 6)
		#define	DPLL_SRC_TXC		(DPLL_TXC << 6)
		#define DPLL_SRC_SET(value)	((value) << 6)
	#define	TXCLK_SRC		FIELD_MASK(3,3)
		#define	TXCLK_NONE			(CLK_NONE << 3)
		#define	TXCLK_RXC			(CLK_RXC << 3)
		#define	TXCLK_TXC			(CLK_TXC << 3)
		#define	TXCLK_DPLL			(CLK_DPLL << 3)
		#define	TXCLK_BRG0			(CLK_BRG0 << 3)
		#define	TXCLK_BRG1			(CLK_BRG1 << 3)
		#define	TXCLK_CTR0_PORT0	(CLK_CTR0 << 3)
		#define	TXCLK_CTR1_PORT1	(CLK_CTR1 << 3)
		#define TXCLK_SRC_SET(value)	(((value) & 0x07) << 3)
	#define	RXCLK_SRC		FIELD_MASK(3,0)
		#define	RXCLK_NONE			(CLK_NONE << 0)
		#define	RXCLK_RXC			(CLK_RXC << 0)
		#define	RXCLK_TXC			(CLK_TXC << 0)
		#define	RXCLK_DPLL			(CLK_DPLL << 0)
		#define	RXCLK_BRG0			(CLK_BRG0 << 0)
		#define	RXCLK_BRG1			(CLK_BRG1 << 0)
		#define	RXCLK_CTR0_PORT0	(CLK_CTR0 << 0)
		#define	RXCLK_CTR1_PORT1	(CLK_CTR1 << 0)
		#define RXCLK_SRC_SET(value)	(((value) & 0x07) << 0)

	/****************************************************************************************/
	// Daisy Chain Control Register (DCCR)
	#define	IUS_OP			FIELD_MASK(2,14)
		#define CLEAR_IUS		(2 << 14)
		#define SET_IUS			(3 << 14)
	#define	RS_IUS			FIELD_MASK(1,13)
	#define	RD_IUS			FIELD_MASK(1,12)
	#define	TS_IUS			FIELD_MASK(1,11)
	#define	TD_IUS			FIELD_MASK(1,10)
	#define	IOP_IUS			FIELD_MASK(1,9)
	#define	MISC_IUS		FIELD_MASK(1,8)
		#define ALL_DCCR_IUS	FIELD_MASK(6,8)
	#define	IP_OP			FIELD_MASK(2,6)
		#define CLEAR_IP_IUS	(1 << 6)		//	only useful for initialization purposes
		#define CLEAR_IP		(2 << 6)
		#define SET_IP			(3 << 6)
	#define RS_IP			FIELD_MASK(1,5)
	#define RD_IP			FIELD_MASK(1,4)
	#define TS_IP			FIELD_MASK(1,3)
	#define TD_IP			FIELD_MASK(1,2)
	#define IOP_IP			FIELD_MASK(1,1)
	#define MISC_IP			FIELD_MASK(1,0)
		#define ALL_DCCR_IP		FIELD_MASK(6,0)

	/****************************************************************************************/
	// DMA Array Count Register (DACR)
	#define RALCNT			FIELD_MASK(4,4)
	#define TALCNT			FIELD_MASK(4,0)

	/****************************************************************************************/
	// DMA Command/Address Register (DCAR)
	#define DCMD		FIELD_MASK(4,12)
		#define NULL_CMD			(0 << 12)
		#define RESET_CHAN			(1 << 12)
		#define START_CHAN			(2 << 12)
		#define START_CONT_CHAN		(3 << 12)
		#define PAUSE_CHAN			(4 << 12)
		#define ABORT_CHAN			(5 << 12)
		#define START_INIT_CHAN		(7 << 12)
		#define RESET_HIGHEST_DMA_IUS	(8 << 12)
		#define RESET_ALL_CHAN			(9 << 12)
		#define START_ALL_CHAN			(10 << 12)
		#define START_CONT_ALL_CHAN		(11 << 12)
		#define PAUSE_ALL_CHAN			(12 << 12)
		#define ABORT_ALL_CHAN			(13 << 12)
		#define START_INIT_ALL_CHAN		(15 << 12)
	#define DMA_CHAN		FIELD_MASK(1,9)
		#define RX_CHAN		(1 << 9)
		#define TX_CHAN		(0 << 9)
	#define MBRE			FIELD_MASK(1,8)
	#define TXRX_REG		FIELD_MASK(1,7)
	// See the CCAR register for bits [6..0] definitions

	/****************************************************************************************/
	// DMA Control Register (DCR)
	#define CHAN_PRI		FIELD_MASK(2,14)
		#define TX_PRI			(0 << 14)
		#define RX_PRI			(1 << 14)
		#define ALTERNATE_PRI	(2 << 14)
	#define PRE_EMPT		FIELD_MASK(1,13)
	#define ALBVO			FIELD_MASK(1,12)
	#define REARBTIME		FIELD_MASK(2,10)
		#define SELECT_CHAN		(0 << 10)
		#define KEEP_CHAN		(1 << 10)
	#define MINOFF39		FIELD_MASK(1,5)
	#define DCSD_OUT		FIELD_MASK(1,4)
	#define ONE_WAIT		FIELD_MASK(1,3)
	#define UAS_ALL			FIELD_MASK(1,2)
	#define ADDR_SEG		FIELD_MASK(2,0)
		#define	INCR_DEC_32		(0 << 0)
		#define INCR_DEC_16		(2 << 0)
		#define INCR_DEC_24		(3 << 0)

	/****************************************************************************************/
	// DMA Interrupt Control Register (DICR)
	#define DMA_MIE			FIELD_MASK(1,15)
	#define DMA_DLC			FIELD_MASK(1,14)
	#define DMA_NV			FIELD_MASK(1,13)
	#define DMA_VIS			FIELD_MASK(1,12)
	#define RX_DMA_IE		FIELD_MASK(1,1)
	#define TX_DMA_IE		FIELD_MASK(1,0)

	/****************************************************************************************/
	// DMA Interrupt Vector Register (DIVR)
	//	Lower 8 bits are Read/write, all other Read only
	#define DMA_IV_READ			FIELD_MASK(5,11)
	#define DMA_TYPE_CODE		FIELD_MASK(2,9)
		#define NO_DMA_PEND		(DMA_NO_PEND << 9)
		#define TX_DMA_PEND		(DMA_TX_EVENT << 9)
		#define RX_DMA_PEND		(DMA_RX_EVENT << 9)
	#define DMA_IV0_READ		FIELD_MASK(1,8)
	#define DMA_INT_VECTOR		FIELD_MASK(8,0)
	
	/****************************************************************************************/
	//	Hardware Configuration Register (HCR)
	//	All bits are Read/Write
	#define	CTR0_DIV		FIELD_MASK(2,14)
		#define	CTR0_DIV32		(0 << 14)
		#define	CTR0_DIV16		(1 << 14)
		#define	CTR0_DIV8		(2 << 14)
		#define	CTR0_DIV4		(3 << 14)
	#define	CTR1_DIV_SEL	FIELD_MASK(1,13)
		#define CTR1_CTR0_DIV	(0 << 13)
		#define CTR1_DPLL_DIV	(1 << 13)
	#define	CVOK			FIELD_MASK(1,12)
	#define	DPLL_DIV		FIELD_MASK(2,10)
		#define	DPLL_DIV32		(0 << 10)
		#define	DPLL_DIV16		(1 << 10)
		#define	DPLL_DIV8		(2 << 10)
		#define CTR1_DIV4		(3 << 10)	// call only be used if the CTR1_DIV_SEL bit is 1, and the DPLL is not used
	#define	DPLL_MODE		FIELD_MASK(2,8)
		#define	DPLL_OFF		(0 << 8)
		#define	DPLL_NRZ		(1 << 8)
		#define	DPLL_BI_MS		(2 << 8)
		#define	DPLL_BIM		(3 << 8)
	#define	BRG1_SINGLE		FIELD_MASK(1,5)
	#define	BRG1_ENAB		FIELD_MASK(1,4)
	#define	BRG0_SINGLE		FIELD_MASK(1,1)
	#define	BRG0_ENAB		FIELD_MASK(1,0)

	/****************************************************************************************/
	// Input/Ouput Control register (IOCR)
	//	All bits are Read/Write
	#define	CTS_MODE		FIELD_MASK(2,14)
		#define	CTS_INP			(0 << 14)	// input
		#define	CTS_LOW			(2 << 14)	// output
		#define	CTS_HIGH		(3 << 14)	// output
	#define	DCD_MODE		FIELD_MASK(2,12)
		#define	DCD_INP			(0 << 12)	// input
		#define	DCD_RXSYNC		(1 << 12)	// input
		#define	DCD_LOW			(2 << 12)	// output
		#define	DCD_HIGH		(3 << 12)	// output
	#define	TXR_MODE		FIELD_MASK(2,10)
		#define	TXR_INP			(0 << 10)	// input
		#define	TXR_TX_DMA		(1 << 10)	// output
		#define	TXR_LOW			(2 << 10)	// output
		#define	TXR_HIGH		(3 << 10)	// output
	#define	RXR_MODE		FIELD_MASK(2,8)
		#define	RXR_INP			(0 << 8)	// input
		#define	RXR_RX_DMA		(1 << 8)	// output
		#define	RXR_LOW			(2 << 8)	// output
		#define	RXR_HIGH		(3 << 8)	// output
	#define	TXD_MODE		FIELD_MASK(2,6)
		#define	TXD_TXD			(0 << 6)	// output
		#define	TXD_HIZ			(1 << 6)	// tri-state
		#define	TXD_LOW			(2 << 6)	// output
		#define	TXD_HIGH		(3 << 6)	// output
	#define	TXC_MODE		FIELD_MASK(3,3)
		#define	TM_TXC_INP			(XC_INP << 3)		// input
		#define	TM_TXC_TXCLK		(TXC_TXCLK << 3)	// output
		#define	TM_TXC_CHAR_CLK		(XC_CHAR_CLK << 3)	// output
		#define	TM_TXC_TXCOMP		(TXC_TXCOMP << 3)	// output
		#define	TM_TXC_BRG0			(XC_BRG0 << 3)		// output
		#define	TM_TXC_BRG1			(XC_BRG1 << 3)		// output
		#define	TM_TXC_CTR1			(TXC_CTR1 << 3)		// output
		#define	TM_TXC_DPLL			(XC_DPLL << 3)		// output
		#define TXC_MODE_SET(value)		(((value) & 0x07) << 3)
	#define	RXC_MODE		FIELD_MASK(3,0)
		#define	RM_RXC_INP			(XC_INP << 0)		// input
		#define	RM_RXC_RXCLK		(RXC_RXCLK << 0)	// output
		#define	RM_RXC_CHAR_CLK		(XC_CHAR_CLK << 0)	// output
		#define	RM_RXC_RXSYNC		(RXC_RXSYNC << 0)	// output
		#define	RM_RXC_BRG0			(XC_BRG0 << 0)		// output
		#define	RM_RXC_BRG1			(XC_BRG1 << 0)		// output
		#define	RM_RXC_CTR0			(RXC_CTR0 << 0)		// output
		#define	RM_RXC_DPLL			(XC_DPLL << 0)		// output
		#define RXC_MODE_SET(value)		(((value) & 0x07) << 0)

	/****************************************************************************************/
	// Interrupt Control register (ICR)
	#define MIE			FIELD_MASK(1,15)
	#define DLC			FIELD_MASK(1,14)
	#define NV			FIELD_MASK(1,13)
	#define VIS			FIELD_MASK(4,9)
		#define	NO_STAT1			(0 << 9)
		#define STAT_ALWAYS			(0x8 << 9)
		#define STAT_EXCEPT_MISC	(0xA << 9)
		#define STAT_TD_TS_RD_RS	(0xB << 9)
		#define STAT_TS_RD_RS		(0xC << 9)
		#define STAT_RD_RS			(0xD << 9)
		#define STAT_RS				(0xE << 9)
		#define NO_STAT2			(0xF << 9)
	#define IE_OP		FIELD_MASK(2,6)
		#define CLEAR_IE		(2 << 6)
		#define SET_IE			(3 << 6)
	#define RS_IE		FIELD_MASK(1,5)
	#define RD_IE		FIELD_MASK(1,4)
	#define TS_IE		FIELD_MASK(1,3)
	#define TD_IE		FIELD_MASK(1,2)
	#define IOP_IE		FIELD_MASK(1,1)
	#define MISC_IE		FIELD_MASK(1,0)

	/****************************************************************************************/
	// Interrupt Vector Register (IVR)
	#define IV_READ			FIELD_MASK(4,12)
	#define TYPE_CODE		FIELD_MASK(3,9)
		#define NO_PEND			(SER_NO_PEND << 9)
		#define MISC_PEND		(SER_MISC_PEND << 9)
		#define IO_PEND			(SER_IO_PEND << 9)
		#define TX_DATA_PEND	(SER_TX_DATA_PEND << 9)
		#define TX_STAT_PEND	(SER_TX_STAT_PEND << 9)
		#define RX_DATA_PEND	(SER_RX_DATA_PEND << 9)
		#define RX_STAT_PEND	(SER_RX_STAT_PEND << 9)
	#define IV0_READ		FIELD_MASK(1,8)
	#define INT_VECTOR		FIELD_MASK(8,0)

	/****************************************************************************************/
	// Miscellaneous Interrupt Status Register (MISR)
	#define RXC_LU			FIELD_MASK(1,15)
	#define RXC_IS			FIELD_MASK(1,14)
	#define TXC_LU			FIELD_MASK(1,13)
	#define TXC_IS			FIELD_MASK(1,12)
	#define RXR_LU			FIELD_MASK(1,11)
	#define RXR_IS			FIELD_MASK(1,10)
	#define TXR_LU			FIELD_MASK(1,9)
	#define TXR_IS			FIELD_MASK(1,8)
	#define DCD_LU			FIELD_MASK(1,7)
	#define DCD_IS			FIELD_MASK(1,6)
	#define CTS_LU			FIELD_MASK(1,5)
	#define CTS_IS			FIELD_MASK(1,4)
		#define ALL_IO_LU		0xAAA0
	#define RCC_UNDER_LU	FIELD_MASK(1,3)
	#define DPLL_SYNC_LU	FIELD_MASK(1,2)
	#define BRG1_LU			FIELD_MASK(1,1)
	#define BRG0_LU			FIELD_MASK(1,0)
		#define ALL_MISC_LU		0x000F
		#define ALL_LU		(ALL_IO_LU | ALL_MISC_LU)

	/****************************************************************************************/
	// Next Receive Address Register Lower (NRARL)

	/****************************************************************************************/
	// Next Receive Byte Count Register (NRBCR)

	/****************************************************************************************/
	// Next Transmit Address Register Lower (NTARL)

	/****************************************************************************************/
	// Next Transmit Address Register Upper (NTARU)

	/****************************************************************************************/
	// Next Transmit Byte Count Register (NTBCR)

	/****************************************************************************************/
	// Port Control Register (PCR)
	//	All bits Read/Write
	#define	P7MODE		FIELD_MASK(2,14)
		#define	P7_GPI		(0 << 14)
		#define	P7_TXCOMP	(1 << 14)	// output
		#define	P7_LOW		(2 << 14)
		#define P7_HIGH		(3 << 14)
	#define	P6MODE		FIELD_MASK(2,12)
		#define	P6_GPI		(0 << 12)
		#define	P6_FSYNC	(1 << 12)	// input
		#define	P6_LOW		(2 << 12)
		#define P6_HIGH		(3 << 12)
	#define	P5MODE		FIELD_MASK(2,10)
		#define	P5_GPI		(0 << 10)
		#define	P5_RXSYNC	(1 << 10)	// output
		#define	P5_LOW		(2 << 10)
		#define P5_HIGH		(3 << 10)
	#define	P4MODE		FIELD_MASK(2,8)
		#define	P4_GPI		(0 << 8)
		#define	P4_TXTSA	(1 << 8)	// output
		#define	P4_LOW		(2 << 8)
		#define P4_HIGH		(3 << 8)
	#define	P3MODE		FIELD_MASK(2,6)
		#define	P3_GPI		(0 << 6)
		#define	P3_RXTSA	(1 << 6)	// output
		#define	P3_LOW		(2 << 6)
		#define P3_HIGH		(3 << 6)
	#define	P2MODE		FIELD_MASK(2,4)
		#define	P2_GPI		(0 << 4)
		#define	P2_LTEN		(1 << 4)	// output
		#define	P2_LOW		(2 << 4)
		#define P2_HIGH		(3 << 4)
	#define	P1MODE		FIELD_MASK(2,2)
		#define	P1_GPI		(0 << 2)
		#define	P1_CLK1		(1 << 2)	// input
		#define	P1_LOW		(2 << 2)
		#define P1_HIGH		(3 << 2)
	#define	P0MODE		FIELD_MASK(2,0)
		#define	P0_GPI		(0 << 0)
		#define	P0_CLK0		(1 << 0)	// input
		#define	P0_LOW		(2 << 0)
		#define P0_HIGH		(3 << 0)

	/****************************************************************************************/
	// Port Status Register (PSR)
	#define P7_LU		FIELD_MASK(1,15)
	#define P7_PS		FIELD_MASK(1,14)
	#define P6_LU		FIELD_MASK(1,13)
	#define P6_PS		FIELD_MASK(1,12)
	#define P5_LU		FIELD_MASK(1,11)
	#define P5_PS		FIELD_MASK(1,10)
	#define P4_LU		FIELD_MASK(1,9)
	#define P4_PS		FIELD_MASK(1,8)
	#define P3_LU		FIELD_MASK(1,7)
	#define P3_PS		FIELD_MASK(1,6)
	#define P2_LU		FIELD_MASK(1,5)
	#define P2_PS		FIELD_MASK(1,4)
	#define P1_LU		FIELD_MASK(1,3)
	#define P1_PS		FIELD_MASK(1,2)
	#define P0_LU		FIELD_MASK(1,1)
	#define P0_PS		FIELD_MASK(1,0)

	/****************************************************************************************/
	// Receive Address Register Lower (RARL)

	/****************************************************************************************/
	// Receive Address Register Upper (RARU)

	/****************************************************************************************/
	// Receive Byte Count Register (RBCR)

	/****************************************************************************************/
	// Receive Character Count Register (RCCR)

	/****************************************************************************************/
	// Receive Command/Status Register (RCSR)
	#define RCMD				FIELD_MASK(4,12)	//Write only
		#define NO_OP				(0 << 12)
		#define CLEAR_RX_CRC		(2 << 12)
		#define ENTER_HUNT			(3 << 12)
		// The next 4 values select the Operation performed in the RICR register, bits [15..8]
		#define SEL_RTSA			(4 << 12)
		#define SEL_RXFIFO_STAT		(5 << 12)
		#define SEL_RINT_LEVEL		(6 << 12)
		#define SEL_RREQ_LEVEL		(7 << 12)
	#define SECOND_BE			FIELD_MASK(1,15)	//Read only
	#define FIRST_BE			FIELD_MASK(1,14)	//Read only
	#define RX_RESIDUE			FIELD_MASK(3,9)
		#define FRAME_ON_CHAR_BOUNDARY	(0 << 9)
	#define SHORTF_CVTYPE		FIELD_MASK(1,8)
	#define EXITED_HUNT			FIELD_MASK(1,7)
	#define IDLE_RECV			FIELD_MASK(1,6)
	#define BREAK_ABORT_RECV	FIELD_MASK(1,5)
	#define RX_BOUND			FIELD_MASK(1,4)
	#define CRCE_FE				FIELD_MASK(1,3)
	#define ABORT_PE			FIELD_MASK(1,2)
	#define RX_OVER				FIELD_MASK(1,1)
	#define RX_AVAIL			FIELD_MASK(1,0)
		#define RX_INT_STATUS(value)	((value) & 0x01FF)
		#define UNLATCH_RCSR_ALL	(SHORTF_CVTYPE | EXITED_HUNT | IDLE_RECV | BREAK_ABORT_RECV | RX_BOUND | ABORT_PE | RX_OVER)

	/****************************************************************************************/
	// Receive Count Limit register (RCLR)
	#define DISABLE_RCC		0x0000
	#define	NO_MAXIMUM		0xFFFF

	/****************************************************************************************/
	//	Receive DMA Interrupt ARM register (RDIAR)
	//	Transmit DMA Interrupt ARM Register (TDIAR)
	#define DMA_INTR_IA		FIELD_MASK(4,0)
		#define EOA_EOL_IA			(1 << 3)
		#define EOB_IA				(1 << 2)
		#define HABORT_IA			(1 << 1)
		#define SABORT_IA			(1 << 0)

	/****************************************************************************************/
	//	Receive DMA Mode Register (RDMR)
	//	Transmit DMA Mode Register (TDMR)
	#define IUSC_DMA_MODE			FIELD_MASK(2,14)
		#define SINGLE_BUF			(0 << 14)
		#define PIPELINED			(1 << 14)
		#define ARRAY				(2 << 14)
		#define LINKED_LIST			(3 << 14)
	#define	TCB_RSB_IN_AL			FIELD_MASK(1,13)
		#define TCB_RSB_IN_DBUF			(0 << 13)
		#define TCB_RSB_IN_ARRAY_LIST	(1 << 13)
	#define CLEAR_COUNT			FIELD_MASK(1,12)
	#define ADDR_MODE			FIELD_MASK(2,10)
		#define INCR_ADDR			(0 << 10)
		#define DECR_ADDR			(1 << 10)
		#define FIX_ADDR			(2 << 10)
	#define TERME				FIELD_MASK(1,9)
	#define TRANS_SIZE			FIELD_MASK(1,8)
		#define TRANS_16			(0 << 8)
		#define TRANS_8				(1 << 8)
	#define START_CONT			FIELD_MASK(1,7)
	#define GLINK				FIELD_MASK(1,6)
	#define BUSY				FIELD_MASK(1,5)
	#define INITG				FIELD_MASK(1,4)
	#define EOA_EOL				FIELD_MASK(1,3)
	#define EOB					FIELD_MASK(1,2)
	#define HABORT				FIELD_MASK(1,1)
	#define SABORT				FIELD_MASK(1,0)
		#define TR_DMA_STATUS		FIELD_MASK(8,0)

	/****************************************************************************************/
	//	Receive Interrupt Control Register (RICR)
	//	Bits [15..8] have many aliases depending on the settings of other register bits
	#define	RTSA_SLOT				FIELD_MASK(7,9)		//	with RCMD=SEL_RTSA and RICR_ALT=0
	#define RTSA_OFFSET				FIELD_MASK(3,13)	//	with RCMD=SEL_RTSA and RICR_ALT=1
		// These TSA macros are also valid for the TICR register
		#define	TSA_SLOT(slot)		((((slot) & 0x7f) << 1) << 8)
		#define	TSA_OFF_CNT(offset,count)	(((((offset) & 7) << 5) | (((count) & 0x0f) << 1) | 1) << 8)
	#define RTSA_COUNT				FIELD_MASK(4,9)		//	with RCMD=SEL_RTSA and RICR_ALT=1
	#define RICR_ALT				FIELD_MASK(1,8)		//	alternate read/write control bit
	#define RXFIFO_COUNT			FIELD_MASK(8,8)		//	with RCMD=SEL_RXFIFO_STAT
	#define RXINT_LEVEL				FIELD_MASK(8,8)		//	with RCMD=SEL_RINT_LEVEL
	#define RXREQ_LEVEL				FIELD_MASK(8,8)		//	with RCMD=SEL_RREQ_LEVEL
		#define READ_INT_REQ_LEVEL(reg)		((reg) >> 8)
		#define SET_INT_REQ_LEVEL(value)	(((value) & 0xFF) << 8)
	#define EXITED_HUNT_IA			FIELD_MASK(1,7)
	#define IDLE_RECV_IA			FIELD_MASK(1,6)
	#define BREAK_ABORT_RECV_IA		FIELD_MASK(1,5)
	#define RX_BOUND_IA				FIELD_MASK(1,4)
	#define WORD_STATUS				FIELD_MASK(1,3)
	#define ABORT_PE_IA				FIELD_MASK(1,2)
	#define RX_OVER_IA				FIELD_MASK(1,1)
		#define RICR_ALL_IA		(EXITED_HUNT_IA | IDLE_RECV_IA | BREAK_ABORT_RECV_IA | RX_BOUND_IA | ABORT_PE_IA | RX_OVER_IA)
	#define TC0R_SEL				FIELD_MASK(1,0)		//	see TICR for TC1R_SEL control bit
		#define TCxR_READ				(0 << 0)
		#define TCxR_CAPTURE			(1 << 0)

	//	These macros allow the CCAR register to be written without disturbing the RTMode bits
	//		the '_iusc_reg' data type is expected for the pointer
	#define SETUP_LOOPBACK(ptr,value,context)		(context)->write_reg_word((void*)&ptr->CCAR.WORD, ((value) & RTMODE))
	#define DO_CCAR_CMD(ptr,cmd,context)			(context)->write_reg_char((void*)&ptr->CCAR.byte.MSB, ((context)->read_reg_char((void*)&ptr->CCAR.byte.MSB) & (RTMODE >> 8)) | (((cmd) >> 8) & ~(RTMODE >> 8)))

//	These macros allow the RICR register to be written without disturbing the WORD_STATUS bit
	//		the '_iusc_reg' data type is expected for the pointer
	//	changes the word status bit
	#define SET_RICR_WORD_STATUS(ptr,value,context)	(context)->write_reg_word((void*)&ptr->RICR.WORD,  ((context)->read_reg_word((void*)&ptr->RICR.WORD) & ~WORD_STATUS) | ((value) & WORD_STATUS))
	//	preserves the WORD STATUS bit contained in the RICR register (all other bits get changed to the new value)
	#define WRITE_RICR_LSB(ptr,value,context)		(context)->write_reg_char((void*)&ptr->RICR.byte.LSB, ((context)->read_reg_char((void*)&ptr->RICR.byte.LSB) & WORD_STATUS) | ((unsigned char)(value) & ~WORD_STATUS))
	#define WRITE_RICR_WORD(ptr,value, context)		(context)->write_reg_word((void*)&ptr->RICR.WORD, ((context)->read_reg_word((void*)&ptr->RICR.WORD) & WORD_STATUS) | ((unsigned short)(value) & ~WORD_STATUS))
   //	These macros allow the LSB of the TICR register to be changed without disturbing the WAIT2SEND bit
	

	/****************************************************************************************/
	// Receive Mode Register (RMR)
	//	All bits Read/Write
	#define	RX_DECODE		FIELD_MASK(3,13)
	//	Many fields and bits same as TMR (see TMR register below)
	#define	QABORT			FIELD_MASK(1,8)
		#define	FOR_ABORT_INDICATION	(1 << 8)	//HDLC mode
		#define	FOR_PARITY_ERROR		(0 << 8)
		#define	RX_ENAB_DCD				(3 << 0)	// for TR_ENAB field

	/****************************************************************************************/
	//	Receive SYNC Register (RSR)
	#define SYN_ADDR_1		FIELD_MASK(8,8)
	#define SYN_ADDR_0		FIELD_MASK(8,0)

	/****************************************************************************************/
	//	Set DMA Interrupt Register (SDIR)
	#define SET_RXDMA_IUS		FIELD_MASK(1,9)
	#define SET_TXDMA_IUS		FIELD_MASK(1,8)
	#define SET_RXDMA_IP		FIELD_MASK(1,1)
	#define SET_TXDMA_IP		FIELD_MASK(1,0)

	/****************************************************************************************/
	//	Status Interrupt Control register (SICR)
	#define RXC_DWN_IA			FIELD_MASK(1,15)
	#define RXC_UP_IA			FIELD_MASK(1,14)
		#define RXC_IA		FIELD_MASK(2,14)
	#define TXC_DWN_IA			FIELD_MASK(1,13)
	#define TXC_UP_IA			FIELD_MASK(1,12)
		#define TXC_IA		FIELD_MASK(2,12)
		#define TRXC_IA		(RXC_IA | TXC_IA)
	#define RXR_DWN_IA			FIELD_MASK(1,11)	//	RXR pin used for RI signal
	#define RXR_UP_IA			FIELD_MASK(1,10)
		#define RXR_IA		FIELD_MASK(2,10)
	#define TXR_DWN_IA			FIELD_MASK(1,9)		//	TXR pin used for DSR signal
	#define TXR_UP_IA			FIELD_MASK(1,8)
		#define TXR_IA		FIELD_MASK(2,8)
	#define DCD_DWN_IA			FIELD_MASK(1,7)		//	DCD signal
	#define DCD_UP_IA			FIELD_MASK(1,6)
		#define DCD_IA		FIELD_MASK(2,6)
	#define CTS_DWN_IA			FIELD_MASK(1,5)		//	CTS signal
	#define CTS_UP_IA			FIELD_MASK(1,4)
		#define CTS_IA		FIELD_MASK(2,4)
	#define RCC_UNDER_IA		FIELD_MASK(1,3)
	#define DPLL_SYNC_IA		FIELD_MASK(1,2)
	#define BRG1_IA				FIELD_MASK(1,1)
	#define BRG0_IA				FIELD_MASK(1,0)
		#define BRG_IA		FIELD_MASK(2,0)

	/****************************************************************************************/
	//	Test Mode Control Register (TMCR)

	/****************************************************************************************/
	//	Test Mode Data Register (TMDR)

	/****************************************************************************************/
	//	Time Constant 0 (zero) Register (TC0R)
	//	Time Constant 1 (one) Register (TC1R)
	
	/****************************************************************************************/
	//	Transmit Address Register Lower (TARL)

	/****************************************************************************************/
	//	Transmit Address Register Upper (TARU)

	/****************************************************************************************/
	//	Transmit Byte Count register (TBCR)

	/****************************************************************************************/
	//	Transmit Character Count Register (TCCR)

	/****************************************************************************************/
	// Transmit Command/Status Register (TCSR)
	#define	TCMD			FIELD_MASK(4,12)
		#define TCSR_ISSUE_TCMD(read_tcsr, context)	((context)->read_reg_word((void*)&read_tcsr) & 0x0F00)	// preserve bits 11..8, and force all other bits to 0
		#define	NO_OP			(0 << 12)
		#define	CLEAR_CRC		(2 << 12)
		// The next 4 values select the Operation performed in the TICR register, bits [15..8]
		#define	SEL_TTSA		(4 << 12)
		#define	SEL_TXFIFO_STAT	(5 << 12)
		#define	SEL_TINT_LEVEL	(6 << 12)
		#define	SEL_TREQ_LEVEL	(7 << 12)
		#define	SEND_FR_MSG		(8 << 12)
		#define	SEND_ABORT		(9 << 12)
		#define	EN_DLE			(12 << 12)
		#define	DIS_DLE			(13 << 12)
		#define	CLEAR_EOFM		(14 << 12)
		#define	SET_EOFM		(15 << 12)
	#define	UNDERWAIT		FIELD_MASK(1,11)
	#define	TXIDLE			FIELD_MASK(3,8)
		#define	TX_IDLE_DEF			(IDLE_DEF << 8)
		#define	TX_IDLE_ALT01		(IDLE_ALT01 << 8)
		#define	TX_IDLE_ZEROS		(IDLE_ZEROS << 8)
		#define	TX_IDLE_ONES		(IDLE_ONES << 8)
		#define	TX_IDLE_MRK_SPC		(IDLE_MRK_SPC << 8)
		#define	TX_IDLE_SPACE		(IDLE_SPACE << 8)
		#define	TX_IDLE_MARK		(IDLE_MARK << 8)
		#define TXIDLE_SET(value)	(((value) & 0x07) << 8)
	#define	PRE_SENT		FIELD_MASK(1,7)
	#define	IDLE_SENT		FIELD_MASK(1,6)
	#define	ABORT_SENT		FIELD_MASK(1,5)
	#define	EOFM_SENT		FIELD_MASK(1,4)
	#define	CRC_SENT		FIELD_MASK(1,3)
	#define	ALL_SENT		FIELD_MASK(1,2)
	#define	TX_UNDER		FIELD_MASK(1,1)
	#define	TX_EMPTY		FIELD_MASK(1,0)
//		#define TX_INT_STATUS(value)	((value) & 0x00FF)
		#define UNLATCH_TCSR_ALL	(PRE_SENT | IDLE_SENT | ABORT_SENT | EOFM_SENT | CRC_SENT | TX_UNDER)

	/****************************************************************************************/
	//	Transmit Count Limit Register (TCLR)

	/****************************************************************************************/
	// Transmit Interrupt Control Register (TICR)
	#define	TTSA_SLOT		FIELD_MASK(7,9)		//	with TCMD=SEL_TTSA and TICR_ALT=0
	#define TTSA_OFFSET		FIELD_MASK(3,13)	//	with TCMD=SEL_TTSA and TICR_ALT=1
	#define TTSA_COUNT		FIELD_MASK(4,9)		//	with TCMD=SEL_TTSA and TICR_ALT=1
	#define	TICR_ALT		FIELD_MASK(1,8)		//	alternate read/write control bit
	#define TXFIFO_COUNT	FIELD_MASK(8,8)		//	with TCMD=SEL_TXFIFO_STAT
	#define TXINT_LEVEL		FIELD_MASK(8,8)		//	with TCMD=SEL_TINT_LEVEL
	#define TXREQ_LEVEL		FIELD_MASK(8,8)		//	with TCMD=SEL_TREQ_LEVEL
	#define	PRE_SENT_IA		FIELD_MASK(1,7)
	#define	IDLE_SENT_IA	FIELD_MASK(1,6)
	#define	ABORT_SENT_IA	FIELD_MASK(1,5)
	#define	EOFM_SENT_IA	FIELD_MASK(1,4)
	#define	CRC_SENT_IA		FIELD_MASK(1,3)
	#define	WAIT2SEND		FIELD_MASK(1,2)
	#define	TX_UNDER_IA		FIELD_MASK(1,1)
	#define	TC1R_SEL		FIELD_MASK(1,0)
		#define TICR_ALL_IA		(PRE_SENT_IA | IDLE_SENT_IA | ABORT_SENT_IA | EOFM_SENT_IA | CRC_SENT_IA | TX_UNDER_IA) 	//	only applies to LSB of register
		//	These macros allow the LSB of the TICR register to be changed without disturbing the WAIT2SEND bit
		#define SET_TICR_WAIT2SEND(ptr,value,context)	(context)->write_reg_char((void*)&(ptr)->TICR.byte.LSB, ((context)->read_reg_char((void*)&ptr->TICR.byte.LSB) & ~WAIT2SEND) | (unsigned char)(value) & WAIT2SEND)
		#define WRITE_TICR_LSB(ptr, value,context)		(context)->write_reg_char((void*)&ptr->TICR.byte.LSB, ((context)->read_reg_char((void*)&ptr->TICR.byte.LSB) & WAIT2SEND) | ((unsigned char)(value) & ~WAIT2SEND))


	/****************************************************************************************/
	// Transmit Mode Register (TMR)
	//	All bit are Read/Write
	//	Many fields and bits in the RMR register are the same as these
	#define	TX_ENCODE		FIELD_MASK(3,13)
		#define	TR_NRZ			(NRZ << 13)
		#define	TR_NRZ_INV		(NRZ_INV << 13)
		#define	TR_NRZI_MARK	(NRZI_MARK << 13)
		#define	TR_NRZI_SPACE	(NRZI_SPACE << 13)
		#define	TR_BIPH_MARK	(BIPH_MARK << 13)
		#define	TR_BIPH_SPACE	(BIPH_SPACE << 13)
		#define	TR_BIPH_LEVEL	(BIPH_LEVEL << 13)
		#define	TR_BIPH_DIFF	(BIPH_DIFF << 13)
		#define TR_ENC(setting)		(((setting) & 0x07) << 13)
	#define	CRC_TYPE		FIELD_MASK(2,11)
		#define	CRC_CCITT		((CCITT-1) << 11)
		#define	CRC_CRC16		((CRC16-1) << 11)
		#define	CRC_CRC32		((CRC32-1) << 11)
		#define CRC_TYPE_SET(value)		((((value)-1) & 0x03) << 11)
	#define	CRC_START	FIELD_MASK(1,10)
		#define	CRC_ZEROS		(0 << 10)
		#define	CRC_ONES		(1 << 10)
		#define CRC_START_SET(value)	(((value) & 0x01) << 10)
	#define	CRC_ENAB		FIELD_MASK(1,9)
	#define	TX_CRC_END		FIELD_MASK(1,8)
	#define	PAR_TYPE		FIELD_MASK(2,6)
		#define	TR_PAR_EVEN		((PAR_EVEN-1) << 6)
		#define	TR_PAR_ODD		((PAR_ODD-1) << 6)
		#define	TR_PAR_SPACE	((PAR_SPACE-1) << 6)
		#define	TR_PAR_MARK		((PAR_MARK-1) << 6)
		#define TR_PAR(setting)	((((setting) - 1) & 0x03) << 6)
	#define	PAR_ENAB		FIELD_MASK(1,5)
	#define	TR_LENGTH			FIELD_MASK(3,2)
		#define	TR_N(nbits)		(((nbits) & 0x07) << 2)	// an 'nbits' value >=8 will yield a setting of zero, which sets 8 bits
	#define	TR_ENAB			FIELD_MASK(2,0)
		#define	TR_DIS_IMM		(0 << 0)
		#define	TR_DIS_EOM		(1 << 0)
		#define	TR_ENAB_UNCON	(2 << 0)
		#define	TR_ENAB_CTS		(3 << 0)
	
	/****************************************************************************************/
	// Transmit Sync Register (TSR)
	//	All bits Read/Write
	#define	SYN1	FIELD_MASK(8,8)
		#define SET_SYN1(char)		(((char) & 0xFF) << 8)
	#define	SYN0	FIELD_MASK(8,0)
		#define SET_SYN0(char)		((char) & 0xFF)
		
	#define FIFO_SIZE				32

	/****************************************************************************************/
	//	Data type defining the Register Map of the IUSC device
	//
	//	Each register is defined by the following union
	//	so that it can be accessed either as
	//	a 16-bit value or as MSB/LSB 8-bit values
	typedef volatile union {
		unsigned short WORD;	//	16-bit access
		struct {
			unsigned char LSB;	//	8-bit, LSB, little endian
			unsigned char MSB;	//	8-bit, MSB, little endian
		} byte;
	} _iusc_reg;

	typedef struct {
		//	IUSC Serial Controller Registers (beginning at 0x000 offset)
		_iusc_reg CCAR;		//	Channel Command/Address
		_iusc_reg CMR;		//	Channel Mode
		_iusc_reg CCSR;		//	Channel Command/Status
		_iusc_reg CCR;		//	Channel Control
		_iusc_reg PSR;		//	Port Status
		_iusc_reg PCR;		//	Port Control
		_iusc_reg TMDR;		//	Test Mode Data
		_iusc_reg TMCR;		//	Test Mode Control
		_iusc_reg CMCR;		//	Clock Mode Control
		_iusc_reg HCR;		//	Hardware Configuration
		_iusc_reg IVR;		//	Interrupt Vector
		_iusc_reg IOCR;		//	Input/Ouput Control
		_iusc_reg ICR;		//	Interrupt Control
		_iusc_reg DCCR;		//	Daisy-Chain Control
		_iusc_reg MISR;		//	Misc. Interrupt Status
		_iusc_reg SICR;		//	Status Interrupt Control
		_iusc_reg RDR;		//	Receive Data (read only)
		_iusc_reg RMR;		//	Receive Mode
		_iusc_reg RCSR;		//	Receive Command/Status
		_iusc_reg RICR;		//	Receive Interrupt Control
		_iusc_reg RSR;		//	Receive Sync
		_iusc_reg RCLR;		//	Receive Count Limit
		_iusc_reg RCCR;		//	Receiver Character Count
		_iusc_reg TC0R;		//	Time Constant Zero(0)
		_iusc_reg TDR;		//	Transmit Data (write only)
		_iusc_reg TMR;		//	Transmit Mode
		_iusc_reg TCSR;		//	Transmit Command/Status
		_iusc_reg TICR;		//	Transmit Interrupt Control
		_iusc_reg TSR;		//	Transmit Sync
		_iusc_reg TCLR;		//	Transmit Count Limit
		_iusc_reg TCCR;		//	Transmit Character Count
		_iusc_reg TC1R;		//	Time Constant One(1)
		unsigned short _gap1[32];	//	aliases of the first 64 bytes
		unsigned short DataFifo[64];	//	Writes go to Transmitter data fifo, Reads comde from Receiver data fifo

		//	IUSC DMA Controller Registers (beginning at 0x100 offset)
		_iusc_reg DCAR;				//	DMA Command/Status
		_iusc_reg TDMR;		//	Transmit DMA Mode
		unsigned short _gap2;
		_iusc_reg DCR;		//	DMA Control
		_iusc_reg DACR;		//	DMA Array Count
		unsigned short _gap3[4];
		_iusc_reg BDCR;		//	Burst/Dwell Control
		_iusc_reg DIVR;		//	DMA Interrupt Vector
		unsigned short _gap4;
		_iusc_reg DICR;		//	DMA Interrupt Control
		_iusc_reg CDIR;		//	Clear DMA Interrupt
		_iusc_reg SDIR;		//	Set DMA Interrupt
		_iusc_reg TDIAR;	//	Transmit DMA Interrupt Arm
		unsigned short _gap5[5];
		_iusc_reg TBCR;		//	Transmit Byte Count
		_iusc_reg TARL;		//	Transmit Address (Lower)
		_iusc_reg TARU;		//	Transmit Address (Upper)
		unsigned short _gap6[5];
		_iusc_reg NTBCR;		//	Next Transmit Byte Count
		_iusc_reg NTARL;		//	Next Transmit Address (Lower)
		_iusc_reg NTARU;		//	Next Transmit Address (Upper)
		unsigned short _gap7[33];
		_iusc_reg RDMR;		//	Receive DMA Mode
		unsigned short _gap8[13];
		_iusc_reg RDIAR;		//	Receive DMA Interrupt Arm
		unsigned short _gap9[5];
		_iusc_reg RBCR;		//	Receive Byte Count
		_iusc_reg RARL;		//	Receive Address (Lower)
		_iusc_reg RARU;		//	Receive Address (Upper)
		unsigned short _gap10[5];
		_iusc_reg NRBCR;		//	Next Receive Byte Count
		_iusc_reg NRARL;		//	Next Receive Address (Lower)
		_iusc_reg NRARU;		//	Next Receive Address (Upper)
		unsigned short _gap11[32];	//	fills to 512 bytes
		//	Board Implemented "Registers"
		unsigned short IntAck;		//	IUSC Interrupt Acknowledge
		unsigned short _gap12[255];	//	fills to 1024 bytes, useful if this structure is used as an array[]
	} _iusc_chip;

	/****************************************************************************************/
	//	Data type for Linked-List Entries (including TCB and RSB)
	//
	typedef struct link_list_entry {
		// !! IMPORTANT NOTE !!
		//	The buffer address ('buffer_addr') is the RAM Address from the IUSC's point of view,
		//		NOT from the Host processors point of view.
		unsigned short *buffer_addr;		// pointer to data buffer
		unsigned short byte_count;			// actual number of bytes to 'transmit from' or 'receive to' this buffer
		// When the TCB and RSB are stored in the List Entry,
		//	the TCBinA/L bit in the TDMR register, and
		//	the RSBinA/L bit in the RDMR register, must be set.
		union {
			unsigned short control;		//	for TCB
			unsigned short status;		//	for RSB
		} csw;
		union {
			unsigned short tcc;			//	for TCB
			unsigned short rcc;			//	for RSB, gets zero'd for 16 bit RSB's
		} trcc;
		// This next value is NOT used by the IUSC-DMA controller
		//	The Transmit DMA channel will read this value, but ignore it.
		//	The Receive DMA channel will zero this value in each List-Entry
		//		in which either the Byte-Count is read as zero, or the Receiver has detected RX-Bound
		unsigned short unused;
		struct link_list_entry *next_entry;
		// Any items added after the above 'next_entry' are not "seen" or used by the IUSC,
		//	but the added length must be allowed for in any Linked List buffer allocation scheme.
		unsigned short buffer_size;
		unsigned short padding;		//	make this structure size evenly divisable by 4
	} LL_entry;

	#define LIST_ENTRY_SIZE		(sizeof(LL_entry))

	

	typedef struct {
		#ifdef	QNX4
		LL_entry __far *TX[2];
		LL_entry __far *RX[2];
		#else
		LL_entry  *TX[2];
		LL_entry  *RX[2];
		#endif
		
	} LL_first;

	typedef enum {
		DMA_TRANSMIT_CHANNEL,	//	0
		DMA_RECEIVE_CHANNEL,	//	1
		DMA_BOTH_CHANNELS		//	2
	} DMA_Chan;

	
	//=============================================================================================
	//	DMA setup parameters
	typedef struct {
		//	Port# (0 to N-1)
		unsigned char port;

		//	DMA mode
		//	Use #defines for bits 8->15 for RDMR and TDMR registers (in file BHNS_iusc.h)
		//	Certain bits are excluded from being set or cleared (see function 'iusc_dma_setup', in file iusc_dma.c)
		unsigned short rx_mode;
		unsigned short tx_mode;

		//	DMA Control
		//	Use #defines for DCR register (in file BHNS_iusc.h)
		//	Certain bits are excluded from being set or cleared (see function 'iusc_dma_setup', in file iusc_dma.c)
		unsigned short control;

		//	Number of FULL positions in the RX FIFO which will initiate a RX DMA transfer
		//	Use a value of 0 (zero) to indicate that the RX DMA is disabled
		//	If both 'rx_int_level' and 'rx_dma_level' are non-zero, 'rx_int_level' will take precedence
		unsigned char rx_dma_level;

		//	Number of EMPTY positions in the TX FIFO which will initiate a TX DMA transfer
		//	Use a value of 0 (zero) to indicate that the TX DMA is disabled
		unsigned char tx_dma_level;

		//	DMA throttling
		//	Use #defines for BDCR register (in file BHNS_iusc.h)
		//	Use caution when using any non-zero value (read chapter 6 of IUSC manual carefully,
		//		there are side effects that effect the integrity of Link-List and Array Entries).
		unsigned short burst_dwell;

	} dma_setup;

	/****************************************************************************************/
	//	Reasonably good set of default settings
	//
	#define BHNS_DMA_SETTINGS_DEFAULT(pnum)	\
		{	pnum,							\
			/* rx_mode */					\
			TCB_RSB_IN_ARRAY_LIST |			\
			LINKED_LIST | INCR_ADDR | CLEAR_COUNT |	TERME,	\
			/* tx_mode */					\
			TCB_RSB_IN_ARRAY_LIST |			\
			LINKED_LIST | INCR_ADDR | CLEAR_COUNT,	\
			/* control */		\
			0,					\
			/* rx_dma_level	*/	\
			16,					\
			/* tx_dma_level	*/	\
			24,					\
			/* burst_dwell */	\
			0					\
		}
		
	/****************************************************************************************/
	//	Interrupt Vector (An 8-bit value obtained by performing the Interrupt Acknowledge bus cycle)
	#define DMA_VECTOR		0x00		//	bit0=0 will indicate a DMA interrupt
	#define SERIAL_VECTOR	0x01		//	bit0=1 will indicate a Serial interrupt
	//		Value for the "TypeCode" field of the respective vector
	#define GET_SER_TYPE(vector)	(((vector) & 0x0E) >> 1)
		#define SER_NO_PEND			0
		#define SER_MISC_PEND		1
		#define SER_IO_PEND			2
		#define SER_TX_DATA_PEND	3
		#define SER_TX_STAT_PEND	4
		#define SER_RX_DATA_PEND	5
		#define SER_RX_STAT_PEND	6
	#define GET_DMA_TYPE(vector)	(((vector) & 0x06) >> 1)
		#define DMA_NO_PEND			0
		#define DMA_TX_EVENT		2
		#define DMA_RX_EVENT		3

	//=============================================================================================
	// Data type defining how to setup the Interrupt mechanisms of the IUSC
	//
	typedef struct {
		//	this is here for convenience (0 -> N-1)
		char port;

		//	Use the #defines for the IA bits in the RICR register (in file CSPCI_iusc.h)
		unsigned char rx_status_ia;

		//	Number of FULL positions in the RX FIFO which cause an interrupt (1 to 32)
		//	Use a value of 0 (zero) to indicate that the RX data interrupt is disabled
		unsigned char rx_int_level;

		//	Use the #defines for the IA bits in the TICR register (in file CSPCI_iusc.h)
		unsigned char tx_status_ia;

		//	Number of EMPTY positions in the TX FIFO which cause an interrupt (1 to 32)
		//	Use a value of 0 (zero) to indicate that the TX data interrupt is disabled
		unsigned char tx_int_level;

		//	Use the #defines for the IA bits in the SICR register (in file CSPCI_iusc.h)
		unsigned short io_misc_ia;

		//	Use the #defines for the IA bits in the RDIAR register (in file CSPCI_iusc.h)
		unsigned char rx_dma_ia;

		//	Use the #defines for the IA bits in the TDIAR register (in file CSPCI_iusc.h)
		unsigned char tx_dma_ia;
	} iusc_int_setup;

	//**************************************************************************
	// A structure that holds information about clock resources that are used
	//	during the setup. These values are used to determine when a resource
	//	is "overloaded" (used more than once with different resulting register settings).
	//
	//	Each item in an array of [2], who's index is used to indicate which resource->..
	//		for example:
	//		used_brg_tc[0] is for the TC value of BRG0, and used_brg_tc[1] is for the TC value of BRG1
	//
	struct _resource{
		unsigned long used_brg_tc[2];
		unsigned char used_brg_src[2];
		unsigned char used_ctr_div[2];
		unsigned char used_ctr_src[2];
		unsigned char ctrs_bypassed;
	} ;
	
	#ifdef QNX4
		 unsigned short write_lifc (int port, void __far *v_pld, void *context, unsigned short new_value);
		 unsigned short read_lifc (int port, void __far *v_pld, void *context);
		 void set_pld_int (int enab, void __far *v_pld, void *context);
		 void iusc_reset (int port, void __far *v_iusc, void __far *v_pld, void *context);
		 void iusc_set_serial_mie (void __far *v_iusc, void *context, int enab);
		 void iusc_set_dma_mie ( void __far *v_iusc, void *context, int enab);
		 int iusc_port_setup (BHN_PortSettings *set, void __far *v_iusc, void __far *v_pld, void *context, char assert_reset, char validate);	
		 int iusc_set_bit_rate (BHN_PortSettings *set, void __far *v_iusc, void __far *v_pld, void *context, int target, unsigned long bps, unsigned char bps_frac, struct _resource *resource);
		 int iusc_get_rx_avail ( void __far *v_iusc, void *context);
		 int iusc_get_tx_space ( void __far *v_iusc, void *context);
		 void iusc_set_rts (void __far *v_iusc, void *context, int state);
		 void iusc_set_dtr (void __far *v_iusc, void *context, int state);
		 int iusc_get_cts (int port, void __far *v_iusc, void *context);
		 int iusc_get_dcd ( void __far *v_iusc, void *context);
		 int iusc_get_dsr ( void __far *v_iusc, void *context);
		 int iusc_get_ri ( void __far *v_iusc, void *context);
		 int iusc_set_li_mode (int port,  void __far *v_iusc, void __far *v_pld, void *context, unsigned char line_mode, unsigned char duplex_mode, unsigned char hflow);
		 unsigned char iusc_get_char(void __far *v_iusc, void *context);
		 void iusc_put_char(int port,  void __far *v_iusc, void *context, unsigned char ch);
		 int iusc_check_char(int port,  void __far *v_iusc, void *context, unsigned char *ch, int operation);
		 int iusc_interrupt_setup (iusc_int_setup *iset, void __far *v_iusc, void __far *v_pld, void *context);
		 void iusc_fake_serial_int (void __far *v_iusc, void *context, unsigned char ip_mask);
		 int iusc_setup_loopback ( void __far *v_iusc, void *context, unsigned short rt_mode);
		 int iusc_send_break( void __far *v_iusc, void *context, long state_duration);	
	#else
		 unsigned short write_lifc (int port, void *v_pld, void *context, unsigned short new_value);
		 unsigned short read_lifc (int port, void *v_pld, void *context);
		 void set_pld_int (int enab, void *v_pld, void *context);
		 void iusc_reset (int port, void *v_iusc, void *v_pld, void *context);
		 void iusc_set_serial_mie (void *v_iusc, void *context, int enab);
		 void iusc_set_dma_mie (void *v_iusc, void *context, int enab);
		 int iusc_port_setup (BHN_PortSettings *set, void *v_iusc, void *v_pld, void *context, char assert_reset, char validate);	
		 int iusc_set_bit_rate (BHN_PortSettings *set, void *v_iusc, void *v_pld, void *context, int target, unsigned long bps, unsigned char bps_frac, struct _resource *resource);
		 int iusc_get_rx_avail (void *v_iusc, void *context);
		 int iusc_get_tx_space (void *v_iusc, void *context);
		 void iusc_set_rts (void *v_iusc, void *context, int state);
		 void iusc_set_dtr (void *v_iusc, void *context, int state);
		 int iusc_get_cts (void *v_iusc, void *context);
		 int iusc_get_dcd (void *v_iusc, void *context);
		 int iusc_get_dsr (void *v_iusc, void *context);
		 int iusc_get_ri (void *v_iusc, void *context);
		 int iusc_set_li_mode (int port,  void *v_iusc, void *v_pld, void *context, unsigned char line_mode, unsigned char duplex_mode, unsigned char hflow);
		 unsigned char iusc_get_char( void *v_iusc, void *context);
		 void iusc_put_char(int port,  void *v_iusc, void *context, unsigned char ch);
		 int iusc_check_char(int port,  void *v_iusc, void *context, unsigned char *ch, int operation);
		 int iusc_interrupt_setup (iusc_int_setup *iset, void *v_iusc, void *v_pld, void *context);
		 void iusc_fake_serial_int (void *v_iusc, void *context, unsigned char ip_mask);
		 int iusc_setup_loopback (void *v_iusc, void *context, unsigned short rt_mode);
		 int iusc_send_break( void *v_iusc, void *context, long state_duration);	
	#endif
	
	
	void iusc_isr_handler (void *context);
	
	
	/****************************************************************************************/
	//	Reasonably good sets of default settings
	//
	#define BHNS_ISR_SETTINGS_FOR_DMA(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,					\
			/* rx_int_level */	\
			0,					\
			/* tx_status_ia */	\
			0,					\
			/* tx_int_level */	\
			0,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			EOA_EOL_IA,			\
			/* tx_dma_ia */		\
			EOA_EOL_IA,			\
		}
	
	#define BHNS_ISR_SETTINGS_TYPICAL(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,					\
			/* rx_int_level */	\
			24,					\
			/* tx_status_ia */	\
			0,					\
			/* tx_int_level */	\
			24,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}

	//=====================================================
	//	Used with the SIMPLE_MONOSYNC port setup
	//		The RX status interrupt IDLE_RECV_IA could be used to
	//		place the Receiver back into Hunt.
	//
	#define ISR_SETTINGS_FOR_SIMPLE_MONOSYNC(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			EXITED_HUNT_IA,		\
			/* rx_int_level */	\
			16,					\
			/* tx_status_ia */	\
			TX_UNDER_IA,		\
			/* tx_int_level */	\
			24,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}

	//=====================================================
	//	An empty settings setup (all interrupt sources disabled)
	//
	#define ISR_SETTINGS_NONE(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,		\
			/* rx_int_level */	\
			0,					\
			/* tx_status_ia */	\
			0,		\
			/* tx_int_level */	\
			0,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}
		
#endif
