/*******************************************************************************
**	File:			cspci_hw.h
**	Author:			Rod Dore, Hasan Jamal
**	
**
**	Description:
**		Board hardware, PLD Settings etc.
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


#ifndef _CSPCI_hw_h
	#define _CSPCI_hw_h

	#define IUSC_OFFSET(portn)	(0x400 * (portn))

	#define PLD_OFFSET			(0x1000)
	#define IUSC_PLD_SIZE		(8 * 1024)
	
	#define SRAM_SIZE			(512 * 1024)	//	number of total bytes

	#define SRAM_OFFSET			0				//	from Aperture-2

	//	size of the PLX "Space-0"
	#define ONE_MEG				(1 * 1024 * 1024)


	#define N_SYNC_PORTS		2

	//	General purpose macro for generating Bit field masks
	#define	FIELD_MASK(size,lowest_bit)		((0xFFFF >> (16-(size))) << (lowest_bit))		//	'size' in bits


	#define	VENDOR_ID		(0x10b5)	//	PLX Vendor ID
	#define	DEVICE_ID		(0x9054)	//	PLX Device ID
	#define	CTI_SUB_VENDOR_ID	(0x12c4)	// CTI's Sub Vendor ID

	/****************************************************************************************/
	//	LEDC: Led Control
	//		All bits are Read/Write (although reading the bits is not really useful)
	#define LED_UNUSED		FILED_MASK(14,1)
	#define LED_DIRECT		FIELD_MASK(1,0)

	/****************************************************************************************/
	//	LIFC: Line Interface Control
	//		All bits are Read/Write
	#define LIFC_UNUSED			FIELD_MASK(9,7)
	#define FOUR_TWO_WIRE		FIELD_MASK(1,5)
		#define DUP_TWO_WIRE		(0 << 5)		// or 1/2 duplex
		#define DUP_FOUR_WIRE		(1 << 5)		// or Multi Drop slave
	#define DUPLEX_MODE			FIELD_MASK(1,4)
		#define DUP_FULL			(0 << 4)
		#define DUP_24				(1 << 4)
	#define TERM_ENAB			FIELD_MASK(1,3)
	#define LINE_MODE			FIELD_MASK(3,0)
		#define LIFC_LM_530A			(LM_530A << 0)	//	RS422 (with V.10 RTS)
		#define LIFC_LM_530				(LM_530 << 0)	//	RS422 (V.11)
		#define LIFC_LM_X21				(LM_X21 << 0)	
		#define LIFC_LM_V35				(LM_V35 << 0)
		#define LIFC_LM_449				(LM_449 << 0)	//	RS449 (V.36)
		#define LIFC_LM_V28				(LM_V28 << 0)	//	RS232 (V.28)
		#define LIFC_LM_SHUTDOWN		(LM_SHUTDOWN << 0)	//	All Inputs/Output on SP508 are Tri-stated
		#define SET_LINE_MODE(mode)		((mode) & 0x07)
	
	/****************************************************************************************/
	//	PLD_CNTRL: PLD Internal Functions Control
	//		All bits are Read/Write
	#define PLD_CNTRL_UNUSED	FIELD_MASK(9,7)
	#define SW_INTERRUPT		FIELD_MASK(1,6)
	#define DMA_ABORT			FIELD_MASK(1,5)
	#define MASTER_IE			FIELD_MASK(1,4)
	#define SOFT_RESET			FIELD_MASK(2,0)
		#define P1_RST				(1 << 0)
		#define P2_RST				(1 << 1)
		#define PN_RST(portn)		(1 << (portn))		// 'portn' is a number from 0 to 1

	/****************************************************************************************/
	//	PLD_STAT: PLD Internal Status
	//		All bits are Read Only
	#define PLD_STAT_UNUSED		FIELD_MASK(2,14) | FIELD_MASK(6,2)
	#define PLD_REV				FIELD_MASK(3,11)
	#define PLD_ID				FIELD_MASK(3,8)
	#define IUSC_INTR_MASK		FIELD_MASK(2,0)
		#define P1_IRQ				(1 << 0)
		#define P2_IRQ				(1 << 1)
		#define PN_IRQ(portn)		(1 << (portn))		// 'portn' is a number from 0 to 1

	/****************************************************************************************/
	//	Data type defining the Register Map of the PLD device

	typedef struct {
		unsigned short cntrl;
		unsigned short _pad;		//	create a 2 byte gap
	} _lif_control;

	typedef struct {
		unsigned short ledc;		//	LED Control
		unsigned short _gap1;		//	create a 4 byte gap
		_lif_control lifc[2];		//	Line Interface Control
		unsigned short pld_cntrl;	//	PLD Internal functions control
		unsigned short _gap2;		//	create a 4 byte gap
		unsigned short pld_status;	//	PLD Internal functions status (read only)
	} _main_pld;


#endif
