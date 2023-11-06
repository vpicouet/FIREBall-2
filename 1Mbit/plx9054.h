/*******************************************************************************
**	File:			plx9054.h
**	Author:			Rod Dore, Hasan Jamal
**	
**
**	Description:
**		PLX 9054 related declarions
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

#ifndef _PLX9054_H
	#define _PLX9054_H
	/**
	**	PLX-9054 definitions
	**/
	#ifdef __IC960
		/**
		**	accessed through the Local Bus side of the 9054
		**/
		#define PLX_REG_BASE		0x80000000		/* Base absolute address */
		#define PLX_BRIDGE_BASE		0x90000000
		#define DRV_CORR			0			/* correction to the offset */
		typedef unsigned long *PLX;				/* convenient data type */
	#endif
	#ifdef CTI_QNX4
		/**
		**	accessed through the PCI Bus via:
		**		PCIBAR0 (memory) address or
		**		PCIBAR1 (I/O address)
		**/
		#define PLX_REG_BASE		0			/* Base relative offset */
		#define DRV_CORR			0x80		/* correction to the offset */
	#endif
	/**
	**	PCI Configuration Registers
	**		(can only be accessed via the Local Bus)
	**/
	#ifdef __IC960
		#define PLX_PCI_VENDOR_ID		(PLX_REG_BASE + 0X00)	/* 16 bits */
		#define PLX_PCI_DEVICE_ID		(PLX_REG_BASE + 0X02)	/* 16 bits */
		#define PLX_PCI_COMMAND			(PLX_REG_BASE + 0X04)	/* 16 bits */
		#define PLX_PCI_STATUS			(PLX_REG_BASE + 0X06)	/* 16 bits */
		#define PLX_REVID				(PLX_REG_BASE + 0X08)	/* 8 bits */
		#define PLX_REG_LVL_PROG		(PLX_REG_BASE + 0X09)	/* 8 bits */
		#define PLX_PCI_SUBCLASS		(PLX_REG_BASE + 0X0A)	/* 8 bits */
		#define PLX_PCI_BASECLASS		(PLX_REG_BASE + 0X0B)	/* 8 bits */
		#define PLX_PCI_CACHE_LINE_SIZE	(PLX_REG_BASE + 0X0C)	/* 8 bits */
		#define PLX_PCI_LATENCY_TIMER	(PLX_REG_BASE + 0X0D)	/* 8 bits */
		#define PLX_PCI_HEADER_TYPE		(PLX_REG_BASE + 0X0E)	/* 8 bits */
		#define PLX_PCI_BIST			(PLX_REG_BASE + 0X0f)	/* 8 bits */
		#define PLX_PCI_BAR0			(PLX_REG_BASE + 0X10)	/* 32 bits */
		#define PLX_PCI_BAR1			(PLX_REG_BASE + 0X14)	/* 32 bits */
		#define PLX_PCI_BAR2			(PLX_REG_BASE + 0X18)	/* 32 bits */
		#define PLX_PCI_BAR3			(PLX_REG_BASE + 0X1C)	/* 32 bits */
		#define PLX_PCI_BAR4			(PLX_REG_BASE + 0X20)	/* 32 bits */
		#define PLX_PCI_BAR5			(PLX_REG_BASE + 0X24)	/* 32 bits */
		#define PLX_PCI_CIS				(PLX_REG_BASE + 0X28)	/* 32 bits */
		#define PLX_PCI_SUBVENDOR_ID	(PLX_REG_BASE + 0X2C)	/* 16 bits */
		#define PLX_PCI_SUBDEV_ID		(PLX_REG_BASE + 0X2E)	/* 16 bits */
		#define PLX_PCI_EXP_ROM			(PLX_REG_BASE + 0X30)	/* 32 bits */
		#define PLX_PCI_NEXT_CAP1		(PLX_REG_BASE + 0X34)	/* 8 bits */
		#define PLX_PCI_INTERRUPT		(PLX_REG_BASE + 0X3c)	/* 8 bits */
		#define PLX_PCI_INT_PIN			(PLX_REG_BASE + 0X3d)	/* 8 bits */
		#define PLX_PCI_MINGNT			(PLX_REG_BASE + 0X3e)	/* 8 bits */
		#define PLX_PCI_MAXLAT			(PLX_REG_BASE + 0X3f)	/* 8 bits */
	#endif
	/**
	**	Local Configuration Registers
	**/
	#define PLX_LAS0RR				(PLX_REG_BASE + 0x80 - DRV_CORR)	/* 32 bits */
	#define PLX_LAS0BA				(PLX_REG_BASE + 0x84 - DRV_CORR)	/* 32 bits */
	#define PLX_MARBR				(PLX_REG_BASE + 0x88 - DRV_CORR)	/* 32 bits */
	#define PLX_DMAARB				(PLX_REG_BASE + 0x12c - DRV_CORR)	/* 32 bits */
	#define PLX_BIGEND				(PLX_REG_BASE + 0x8c - DRV_CORR)	/* 8 bits */
	#define PLX_LMISC				(PLX_REG_BASE + 0x8d - DRV_CORR)	/* 8 bits */
	#define PLX_PROT_AREA			(PLX_REG_BASE + 0x8e - DRV_CORR)	/* 16 bits */
	#define PLX_EROMRR				(PLX_REG_BASE + 0x90 - DRV_CORR)	/* 32 bits */
	#define PLX_EROMBA				(PLX_REG_BASE + 0x94 - DRV_CORR)	/* 32 bits */
	#define PLX_LBRD0				(PLX_REG_BASE + 0x98 - DRV_CORR)	/* 32 bits */
	#define PLX_DMRR				(PLX_REG_BASE + 0x9c - DRV_CORR)	/* 32 bits */
	#define PLX_DLMBAM				(PLX_REG_BASE + 0xA0 - DRV_CORR)	/* 32 bits */
	#define PLX_DLMBAI				(PLX_REG_BASE + 0xA4 - DRV_CORR)	/* 32 bits */
	#define PLX_DMPBAM				(PLX_REG_BASE + 0xA8 - DRV_CORR)	/* 32 bits */
	#define PLX_DMCFGA				(PLX_REG_BASE + 0xAc - DRV_CORR)	/* 32 bits */
	#define PLX_LAS1RR				(PLX_REG_BASE + 0x170 - DRV_CORR)	/* 32 bits */
	#define PLX_LAS1BA				(PLX_REG_BASE + 0x174 - DRV_CORR)	/* 32 bits */
	#define PLX_LBRD1				(PLX_REG_BASE + 0x178 - DRV_CORR)	/* 32 bits */
	#define PLX_DMDAC				(PLX_REG_BASE + 0x17c - DRV_CORR)	/* 32 bits */
	/**
	**	Run-Time Registers
	**/
	#define PLX_MBOX0				(PLX_REG_BASE + 0xC0 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX1				(PLX_REG_BASE + 0xC4 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX2				(PLX_REG_BASE + 0xC8 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX3				(PLX_REG_BASE + 0xCc - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX4				(PLX_REG_BASE + 0xD0 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX5				(PLX_REG_BASE + 0xD4 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX6				(PLX_REG_BASE + 0xD8 - DRV_CORR)	/* 32 bits */
	#define PLX_MBOX7				(PLX_REG_BASE + 0xDc - DRV_CORR)	/* 32 bits */
	#define PLX_P2LDBELL			(PLX_REG_BASE + 0xE0 - DRV_CORR)	/* 32 bits */
	#define PLX_L2PDBELL			(PLX_REG_BASE + 0xE4 - DRV_CORR)	/* 32 bits */
	#define PLX_INTCSR				(PLX_REG_BASE + 0xE8 - DRV_CORR)	/* 32 bits */
	#define PLX_CNTRL				(PLX_REG_BASE + 0xEc - DRV_CORR)	/* 32 bits */
	#define PLX_PCIHIDR				(PLX_REG_BASE + 0xF0 - DRV_CORR)	/* 32 bits */
	#define PLX_PCIHREV				(PLX_REG_BASE + 0xF4 - DRV_CORR)	/* 8 bits */
	/**
	**	DMA Registers
	**/
	#define PLX_DMAMODE0			(PLX_REG_BASE + 0x100 - DRV_CORR)	/* 32 bits */
	#define PLX_DMAPADR0			(PLX_REG_BASE + 0x104 - DRV_CORR)	/* 32 bits */
	#define PLX_DMALADR0			(PLX_REG_BASE + 0x108 - DRV_CORR)	/* 32 bits */
	#define PLX_DMASIZ0				(PLX_REG_BASE + 0x10c - DRV_CORR)	/* 32 bits */
	#define PLX_DMADPR0				(PLX_REG_BASE + 0x110 - DRV_CORR)	/* 32 bits */
	#define PLX_DMAMODE1			(PLX_REG_BASE + 0x114 - DRV_CORR)	/* 32 bits */
	#define PLX_DMAPADR1			(PLX_REG_BASE + 0x118 - DRV_CORR)	/* 32 bits */
	#define PLX_DMALADR1			(PLX_REG_BASE + 0x11c - DRV_CORR)	/* 32 bits */
	#define PLX_DMASIZ1				(PLX_REG_BASE + 0x120 - DRV_CORR)	/* 32 bits */
	#define PLX_DMADPR1				(PLX_REG_BASE + 0x124 - DRV_CORR)	/* 32 bits */
	#define PLX_DMACSR0				(PLX_REG_BASE + 0x128 - DRV_CORR)	/* 8 bits */
	#define PLX_DMACSR1				(PLX_REG_BASE + 0x129 - DRV_CORR)	/* 8 bits */
	#define PLX_DMATHR				(PLX_REG_BASE + 0x130 - DRV_CORR)	/* 32 bits */
	#define PLX_DMADAC0				(PLX_REG_BASE + 0x134 - DRV_CORR)	/* 32 bits */
	#define PLX_DMADAC1				(PLX_REG_BASE + 0x138 - DRV_CORR)	/* 32 bits */
	/**
	**	Messaging Queue Registers
	**/
	#define PLX_OPQIS				(PLX_REG_BASE + 0xB0 - DRV_CORR)	/* 32 bits */
	#define PLX_OPQIM				(PLX_REG_BASE + 0xB4 - DRV_CORR)	/* 32 bits */
	#ifdef CTI_QNX4
		#define PLX_IQP					(PLX_REG_BASE + 0x40)	/* 32 bits */
		#define PLX_OQP					(PLX_REG_BASE + 0x44)	/* 32 bits */
	#endif
	#define PLX_MQCR				(PLX_REG_BASE + 0x140 - DRV_CORR)	/* 32 bits */
	#define PLX_QBAR				(PLX_REG_BASE + 0x144 - DRV_CORR)	/* 32 bits */
	#define PLX_IFHPR				(PLX_REG_BASE + 0x148 - DRV_CORR)	/* 32 bits */
	#define PLX_IFTPR				(PLX_REG_BASE + 0x14c - DRV_CORR)	/* 32 bits */
	#define PLX_IPHPR				(PLX_REG_BASE + 0x150 - DRV_CORR)	/* 32 bits */
	#define PLX_IPTPR				(PLX_REG_BASE + 0x154 - DRV_CORR)	/* 32 bits */
	#define PLX_OFHPR				(PLX_REG_BASE + 0x158 - DRV_CORR)	/* 32 bits */
	#define PLX_OFTPR				(PLX_REG_BASE + 0x15c - DRV_CORR)	/* 32 bits */
	#define PLX_OPHPR				(PLX_REG_BASE + 0x160 - DRV_CORR)	/* 32 bits */
	#define PLX_OPTPR				(PLX_REG_BASE + 0x164 - DRV_CORR)	/* 32 bits */
	#define PLX_QSR					(PLX_REG_BASE + 0x168 - DRV_CORR)	/* 32 bits */

	//	Here's another way of defining the PLX registers
	typedef struct __plx_registers {
		//	Local Configuration Registers
		unsigned long LAS0RR;			//	Offset 0
		unsigned long LAS0BA;			//	4
		unsigned long MARBR;			//	8	//	same as DMAARB
		unsigned char BIGEND;			//	C
		unsigned char LMISC;			//	D
		unsigned short PROT_AREA;		//	E
		unsigned long EROMRR;			//	10
		unsigned long EROMBA;			//	14
		unsigned long LBRD0;			//	18
		unsigned long DMRR;				//	1C
		unsigned long DLMBAM;			//	20
		unsigned long DLMBAI;			//	24
		unsigned long DMPBAM;			//	28
		unsigned long DMCFGA;			//	2C
		//	Messaging Queue Registers
		unsigned long OPQIS;			//	30
		unsigned long OPQIM;			//	34
		unsigned char pad1[8];			//	38
		union {
			unsigned long IQP;			//	40	//	PCI access only
			unsigned long MBOX0;		//	40	//	Local Bus only
		} IQP_MBOX0;
		union {
			unsigned long OQP;			//	44	//	PCI access only
			unsigned long MBOX1;		//	44	//	Local Bus only
		} OQP_MBOX1;
		unsigned long MBOX2;			//	48
		unsigned long MBOX3;			//	4C
		unsigned long MBOX4;			//	50
		unsigned long MBOX5;			//	54
		unsigned long MBOX6;			//	58
		unsigned long MBOX7;			//	5C
		unsigned long P2LDBELL;			//	60
		unsigned long L2PDBELL;			//	64
		unsigned long INTCSR;			//	68
		unsigned long CNTRL;			//	6C
		unsigned long PCIHIDR;			//	70
		unsigned char PCIHREV;			//	74
		unsigned char pad2[3];			//	75
		unsigned long MBOX0;			//	78	//	Alternate location
		unsigned long MBOX1;			//	7C	//	Alternate location
		unsigned long DMAMODE0;			//	80
		unsigned long DMAPADR0;			//	84
		unsigned long DMALADR0;			//	88
		unsigned long DMASIZ0;			//	8C
		unsigned long DMADPR0;			//	90
		unsigned long DMAMODE1;			//	94
		unsigned long DMAPADR1;			//	98
		unsigned long DMALADR1;			//	9C
		unsigned long DMASIZ1;			//	A0
		unsigned long DMADPR1;			//	A4
		unsigned char DMACSR0;			//	A8
		unsigned char DMACSR1;			//	A9
		unsigned char pad3[2];			//	AA
		unsigned long DMAARB;			//	AC	//	same as MARBR
		unsigned long DMATHR;			//	B0
		unsigned long DMADAC0;			//	B4
		unsigned long DMADAC1;			//	B8
		unsigned char pad4[4];			//	BC
		unsigned long MQCR;				//	C0
		unsigned long QBAR;				//	C4
		unsigned long IFHPR;			//	C8
		unsigned long IFTPR;			//	CC
		unsigned long IPHPR;			//	D0
		unsigned long IPTPR;			//	D4
		unsigned long OFHPR;			//	D8
		unsigned long OFTPR;			//	DC
		unsigned long OPHPR;			//	E0
		unsigned long OPTPR;			//	E4
		unsigned long QSR;				//	E8
		unsigned char pad5[4];			//	EC
		unsigned long LAS1RR;			//	F0
		unsigned long LAS1BA;			//	F4
		unsigned long LBRD1;			//	F8
		unsigned long DMDAC;			//	FC
	} _PLX_registers;
#endif
