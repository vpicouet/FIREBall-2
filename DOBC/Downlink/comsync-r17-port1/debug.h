/*******************************************************************************
**	File:			debug.h
**	Author:			Hasan Jamal
**	
**
**	Description:
**		Helper functions for debug 
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

#ifndef _comsync_debug_h
	#define _comsync_debug_h
	
#define PRINT_HEAD     "comsync: "
#define DBGPRINT_HEAD  "comsyncD: "

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define DEBUG_LEVEL_DEFAULT  0x00000000

#define DEBUG_INIT     0x00000001
#define DEBUG_READ     0x00000002
#define DEBUG_WRITE    0x00000004
#define DEBUG_IOCTL    0x00000008
#define DEBUG_OPENCLS  0x00000010
#define DEBUG_PSET     0x00000020
#define DEBUG_INTR     0x00000040
#define DEBUG_TXSTAT   0x00000080
#define DEBUG_RXSTAT   0x00000100
#define DEBUG_MISCSTAT  0x00000200
#define DEBUG_IOSTAT  0x00000400
#define DEBUG_ALWAYS   0x10000000
extern unsigned long  __debug_level;

#define DBGPRINTNOHEAD(lvl, fmt, ...)  \
	do {  \
		if((lvl) & __debug_level)  \
			printk(fmt, ##__VA_ARGS__);  \
	} while(0)
#define DBGPRINT(lvl, klvl, fmt, ...)  \
	do {  \
		if((lvl) & __debug_level)  \
			printk(klvl DBGPRINT_HEAD fmt, ##__VA_ARGS__);  \
	} while(0)
#else
#define DBGPRINTNOHEAD(lvl, fmt, ...)
#define DBGPRINT(lvl, klvl, fmt, ...)
#endif

#define PRINTNOHEAD(fmt, ...)  \
	do {  \
		printk(fmt, ##__VA_ARGS__);  \
	} while(0)
#define PRINT(klvl, fmt, ...)  \
	do {  \
		printk(klvl PRINT_HEAD fmt, ##__VA_ARGS__);  \
	} while(0)

#endif
