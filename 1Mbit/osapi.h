/*******************************************************************************
**	File:			osapi.h
**	Author:			Hasan Jamal
**
**
**	Description:
**		OS specific I/O routine declarations that will be used by function pointer
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

#ifndef _comsync_api_h
	#define _comsync_api_h

struct _api_context {
	unsigned char (*read_reg_char)(void *address);
	unsigned short (*read_reg_word)(void *address);
	unsigned long  (*read_reg_long)(void *address);

	void  (*write_reg_char)(void *address, unsigned char value);
	void  (*write_reg_word)(void *address, unsigned short value);
	void  (*write_reg_long)(void *address, unsigned long value);
};

unsigned char CTI_read_char(void *address);
unsigned short CTI_read_word(void *address);
unsigned long CTI_read_long(void *address);
void CTI_write_char(void *address, unsigned char value);
void CTI_write_word(void *address, unsigned short value);
void CTI_write_long(void *address, unsigned long value);

unsigned char CTI_read_io_char(void *address);
unsigned short CTI_read_io_word(void *address);
unsigned long CTI_read_io_long(void *address);
void CTI_write_io_char(void *address, unsigned char value);
void CTI_write_io_word(void *address, unsigned short value);
void CTI_write_io_long(void *address, unsigned long value);

#endif
