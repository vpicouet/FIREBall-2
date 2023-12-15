/*******************************************************************************
**	File:			osapi.c
**	Author:			Hasan Jamal
**
**
**	Description:
**		OS specific I/O routines that will be used by function pointer
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

#include <asm/io.h>
#include <linux/types.h>

unsigned char CTI_read_char(void __iomem *address)
{
	//printk("readb: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return readb(address);
	#endif
}

unsigned short CTI_read_word(void __iomem *address)
{
	//printk("readw: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return readw(address);
	#endif
}

unsigned long CTI_read_long(void __iomem *address)
{
	//printk("readl: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return readl(address);
	#endif
}

void CTI_write_char(void __iomem *address, unsigned char value)
{
	//printk("writeb: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	writeb(value, address);
	#endif
}

void CTI_write_word(void __iomem *address, unsigned short value)
{
	//printk("writew: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	writew(value, address);
	#endif
}

void CTI_write_long(void __iomem *address, unsigned long value)
{
	//printk("writel: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	writel(value, address);
	#endif
}

/*****************************************************************************************/
unsigned char CTI_read_io_char(unsigned short address)
{
	//printk("readb: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return inl((unsigned)address);
	#endif
}

unsigned short CTI_read_io_word(unsigned short address)
{
	//printk("readw: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return inw((unsigned)address);
	#endif
}

unsigned long CTI_read_io_long(unsigned short address)
{
	//printk("readl: address %x \n",address);
	if(!address)
		return 0;
	#ifdef LINUX
	return inl((unsigned)address);
	#endif
}

void CTI_write_io_char(unsigned short address, unsigned char value)
{
	//printk("writeb: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	outb(value, (unsigned)address);
	#endif
}

void CTI_write_io_word(unsigned short address, unsigned short value)
{
	//printk("writew: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	outw(value, (unsigned)address);
	#endif
}

void CTI_write_io_long(unsigned short address, unsigned long value)
{
	//printk("writel: address %x value %x\n",address, value);
	if(!address)
		return;
	#ifdef LINUX
	outl(value, (unsigned)address);
	#endif
}
