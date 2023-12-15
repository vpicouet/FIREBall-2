/*******************************************************************************
**	File:			ioctls.h
**	Author:			Hasan Jamal
**	
**
**	Description:
**		ioctls declaration include file
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

#ifndef _comsync_drv_ioctls_h
	#define _comsync_drv_ioctls_h

#define TIOGETPORTSET      0x54A1  /* Get sync port settings */
#define TIOSETPORTSET      0x54A2  /* Set sync port settings */
#define TIOHUNT            0x54A3  /* Start receiver hunt mode */
#define TIOLOOPBACK        0x54A4  /* loopback for dignostic */
#define TIOGETEVENT        0x54A5  /* Get event mask */
#define TIOCSERGETMSR      0x54A6  /* Get the MSR contents */
#define TIOSETCLRRTS	   0x54A8  /* Set/Clear RTS signal */	
#define TIOSETCLRDTR	   0x54A9  /* Set/Clear DTR signal */	
#define TIOSETCLRBRK	   0x54AA  /* Set/Clear break signal */	
#define TIOGETREGISTER	   0x54AB  /* Set/Clear break signal */	
#define TIOSETREGISTER	   0x54AC  /* Set/Clear break signal */	

// structure to set ESCC/IUSC register value
struct set_reg_struct {
        unsigned char value; //value to set
        int offset; // offset of the register
	};

#endif
