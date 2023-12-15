/*******************************************************************************
**	File:			PRF5123_hw.h
**	Author:			Rod Dore
**
**
**	Description:
**		ComsyncIII hardware speicic structures.
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

#ifndef _PRF5123_hw_h
	#define _PRF512_hw_h

	#define	COMSYNC_III_VENDOR_ID			(0x11AA)	//	Actel Vendor ID
	#define	DUT_DEVICE_ID		(0x1770)	//	Actel Device ID
//	#define HELPER_DEVICE_ID	(0x1771)	//	Helper board device ID
	#define	CTI_SUB_VENDOR_ID	(0x12c4)	//	CTI's Sub Vendor ID
	#define CTI_DEVICE_ID		(0x0820)	//	Board's Device ID

	#define NUM_BARS			3		//	PCI BAR's
	#define	ESCC_REGS_BAR_size		256
	#define	ESCC_FIFO_BAR_size		2048
	#define	PCI_CORE_BAR_size		256

	
	
	typedef	struct {
		unsigned char	data;
		unsigned char	control;
	} _ESCC_raw_access;

	typedef struct {
//		unsigned long	phy_base;		//	Physical memory address (32 bits)
//	Rod, Nov.29,2012, reverse their order, to match new FPGA code
		unsigned long	trans_cnt;		//	Transfer count (16 bits used)
		unsigned long	phy_base;		//	Physical memory address (32 bits)
	} _dma_setup;

	typedef struct {
		_dma_setup	tx[2];
		_dma_setup	rx[2];
	} _dma_port;


#pragma pack(1)
	typedef volatile struct _bar_zero {
		//-----------------------------------------------------------------
		//	ESCC direct access
		//		Offset 0x00
		_ESCC_raw_access	ESCC_Raw[2];	//	Offsets: 0->3

		//-----------------------------------------------------------------
		//	Board registers
		//		Starting at Offset  0x04
		union {
			struct {
				byte DtrA:1;			// DTR, channel A
				byte DtrB:1;			// DTR, channel B
				byte :6;				//	unused bits
			} _ctrl;					// Write only
			struct {
				byte DsrA:1;			// DSR, channel A
				byte DsrB:1;			// DSR, channel B
				byte :1;				// unused bit
				byte escc_int:1;			// SCC interrupt (active low)
				byte :4;				// unused bits
			} _stat;					// Read only
			byte reg;
		} _sc;							// Offset 4: Status & Control

		unsigned char	fill_1;			//	Offset 5: Not used (read as 0xEE)

		union {
			byte escc_intack;			// Read: Interrupt acknowledge cycle to ESCC
			byte escc_rst;				// Write: Reset ESCC (any value works)
		} _eri;							// Offset 6: Misc.2 I/O


//		unsigned char	misc_control;

		union {
			struct {
				byte LimA:3;			// Line Interface mode, channel A
				byte LimB:3;			// Line Interface mode, channel B
				byte mie:1;				// Master interrupt enable
				byte :1;				// unused bit
			} bf;
			byte reg;
		} _misc;						// Offset 7: Misc.3 I/O

		unsigned char	fill_2;			//	Offset 8: Not used (reads as 0xEE)

		union {
			struct {
				byte RS485_RXenA:1;		// Enable RTS to control RX enable, Channel A
				byte RS485_TXtriA:1;	// Enable RTS to control TX tri-state, Channel A
				byte RS485_RXenB:1;		// Enable RTS to control RX enable, Channel B
				byte RS485_TXtriB:1;	// Enable RTS to control TX tri-state, Channel B
				byte fast_srA:1;		//	Fast Slew rate, Channel A
				byte fast_srB:1;		//	Fast Slew rate, Channel B
				byte :2;				// unused bits
			} bf;
			byte reg;
		} _rs485;						// Offset 9:

		unsigned char	fill_3[22];		//	Offset 0x0A to 0x1F

		//-----------------------------------------------------------------
		//	ESCC aliased regsiter access
		//		Starting at Offset 0x20 -> 0x3F
		_scc			ESCC_Alias[2];

		//-----------------------------------------------------------------
		// DMA Control
		//		Starting at offset 0x40
		_dma_port	DMA[2];						//	DMA address/transfer regs (one set per port)
		unsigned long	dma_control;			//	DMA control (commands)
		unsigned long	dma_status;				//	DMA Status
		unsigned long	dma_interrupt;			//	DMA Interrupts
		unsigned long	tx_throttle[2];			//	Transmitter transfer throttle
		unsigned long	rx_ene_timeout[2];		//	Receiver empty / not-empty timeout
		unsigned long	master_intr_enab;		//	Master interrupt enable
		unsigned long	master_intr_stat;		//	Master interrupt status
		unsigned long	fifo_intr_stat;			//	Fifo's interrupt status & state
		unsigned long	fifo_intr_enab;			//	Fifo interupts enable
		unsigned long	timestamp;				//	revolving 32 bit counter (read only)

	} _Bar_0;
	
	//	DMA Control
	#define ABORT_TX_DMA(P)		(0x1L << (P*2))		//	P(port) is zero based
	#define	ABORT_RX_DMA(P)		(0x2L << (P*2))

	//	DMA Status
	#define	TX_BUF_BUSY(P,B)	(0x100L << ((P*4)+(B*1)))	//	P(port), B(buffer) are zero based
	#define	TX_BUFS_BUSY(P)		(0x300L << (P*4))
	#define	RX_BUF_BUSY(P,B)	(0x400L << ((P*4)+(B*1)))
	#define	RX_BUFS_BUSY(P)		(0xC00L << (P*4))
	#define	TX_BUF(P)			(0x10000L << (P*2))		//	P(port) is zero based
	#define	RX_BUF(P)			(0x20000L << (P*2))

	//	DMA Interrupts
	#define	TX_EOB(P)			(0x1L << (P*4))		//	P(port) is zero based
	#define	TX_EOL(P)			(0x2L << (P*4))
	#define	TX_EOB_EOL(P)		(0x3L << (P*4))
	#define	RX_EOB(P)			(0x4L << (P*4))
	#define	RX_EOL(P)			(0x8L << (P*4))
	#define	RX_EOB_EOL(P)		(0xCL << (P*4))
	#define	ALL_EOB_EOL			(0xFFL)

	//	TX Throttle
	#define	TX_Throttle(bit_rate)	(16000000L / bit_rate)

	//	RX Empty (Not Enpty)
	#define RX_Ene(bit_rate)	(20000000L / bit_rate)

	//	Master Interrupt Enable
	#define	ESCC_INTR_ENAB	(0x1L)
	#define	DMA_INTR_ENAB	(0x2L)
	#define	FIFO_INTR_ENAB	(0x4L)
	
	//	Master Interrupt Status
	#define	ESCC_INTR	(0x1L)
	#define	DMA_INTR	(0x2L)
	#define	FIFO_INTR	(0x4L)

	//	FIFO Interrupt Status
	#define	RX_ENE_TO_EVENT(P)	(0x1L << (P*4))		//	P(port) is zero based
	#define	RX_AF_EVENT(P)		(0x2L << (P*4))
	#define	TX_E_EVENT(P)		(0x4L << (P*4))
	#define	TX_AE_EVENT(P)		(0x8L << (P*4))
	#define	RELEASE_RX_HALT(P)	(0x100L << (P*1))
	#define TX_E(P)				(0x10000L << (P*8))
	#define TX_AE(P)			(0x20000L << (P*8))
	#define TX_AF(P)			(0x40000L << (P*8))
	#define TX_F(P)				(0x80000L << (P*8))
	#define RX_E(P)				(0x100000L << (P*8))
	#define RX_AE(P)			(0x200000L << (P*8))
	#define RX_AF(P)			(0x400000L << (P*8))
	#define RX_F(P)				(0x800000L << (P*8))

	//	FIFO Interrupt Enable
	#define	RX_ENE_TO_ENAB(P)	(0x1L << (P*4))
	#define	RX_AF_ENAB(P)		(0x2L << (P*4))
	#define	TX_E_ENAB(P)		(0x4L << (P*4))
	#define	TX_AE_ENAB(P)		(0x8L << (P*4))

	typedef struct  {
		unsigned char	tx[512];
		unsigned char	rx[512];
	} _data_fifos;

	typedef volatile struct _bar_one {
		_data_fifos		port_fifo[2];
	} _Bar_1;

	typedef volatile struct _bar_two {
		unsigned long	pci_config[16];		//	Standard PCI Configuration area (0x00 -> 0x3F)
		unsigned long	upper_config[3];
		unsigned long	fifo_status;
		unsigned long	dma_pci_address;
		unsigned long	dma_backend_address;
		unsigned long	dma_transfer_count;
		union {
			unsigned long	dma_control_status;
			unsigned char	byte[4];
		} dma_cont_stat;
	} _Bar_2;
	
#pragma pack()
	//	DMA Control Status
	#define	BACKEND_INT_STAT	(0x40)

#endif
