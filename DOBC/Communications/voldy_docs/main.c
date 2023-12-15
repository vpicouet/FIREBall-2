/*******************************************************************************
**	File:			main.c
**	Author:			Hasan Jamal
**
**
**	Description:
**		dispatch routines for Comsync linux driver
**
**	Copyright (c) 2016, CTI, Connect Tech Inc. All Rights Reserved.
**
**	THIS IS THE UNPUBLISHED PROPRIETARY SOURCE CODE OF CONNECT TECH INC.
**	The copyright notice above does not evidence any actual or intended
**	publication of such source code.
**
**	This module contains Proprietary Information of Connect Tech, Inc
**	and should be treated as Confidential.
******************************************************************************/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/irq.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <linux/list.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <asm/termios.h>
#include<linux/version.h>
#include <linux/poll.h>
#include <linux/ioport.h>
#include <linux/syscalls.h>


#include "precomp.h"


MODULE_AUTHOR("HJ");


// some helpful macros from linux driver book
#define TYPE(dev) (MINOR(dev) >> 4) /* high nibble */
#define NUM(dev) (MINOR(dev) & 0xf) /* low nibble */
// end of macros

unsigned long  __debug_level = DEBUG_ALWAYS | DEBUG_LEVEL_DEFAULT;// | DEBUG_TXSTAT | DEBUG_INTR | DEBUG_WRITE |DEBUG_TXSTAT|DEBUG_MISCSTAT|DEBUG_IOSTAT;


// driver's default major number
static int comsync_major = 9;
// major number is also an argument to driver startup to suit the major number for a system
module_param(comsync_major, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);


#define COMSYNC_DRV_NAME "csync"
static char *comsync_name = "Comsync driver";
static char *comsync_version = "version 1.15 July 28 2017";


const int BUF_SIZE 		=	1024 * 32; //32K


// global structure for bus_driver_struct
struct bus_driver_struct *bus_driver[MAX_COMSYNC_BOARD];

// global variable to hold total number of comsync boards in the system
int num_comsync_board = 0;

struct timer_list sync_timer;
// flag to indicate whether rx_char_scoop_timer is launched
unsigned int async_rx_timer;

// I/O base address to be provided as a driver argument for Comsync/104
static unsigned int iobase1 = 0;
static unsigned int iobase2 = 0;
static unsigned int iobase3 = 0;
static unsigned int iobase4 = 0;

// I/O base address to be provided as a driver argument for Comsync/104
static unsigned int irq1 = 0;
static unsigned int irq2 = 0;
static unsigned int irq3 = 0;
static unsigned int irq4 = 0;


void tx_escc(
	 struct sync_port_drv_struct *sync_port,
	 char *WriteCurrentChar,
	 unsigned long Length
	);

void escc_tx_fifo(struct sync_port_drv_struct *sync_port);

#define MAX_ISA_DMA		8

void start_DMA_write(struct sync_port_drv_struct *sync_port);

//helper routine to print ISUC register values for debugging puposes
//void print_iusc_reg(struct sync_port_drv_struct *sync_port)
//{
//	int i;
//	char *tmp;
//
//	tmp = (char*)sync_port->iusc;
//
//	for(i=0;i<0x3e;i+=2)
//	{
//		printk("IUSC 0x%x addr 0x%x val 0x%x\n",i,tmp,sync_port->api.read_reg_word((void *)tmp));
//		tmp+=2;
//	}
//}


// open function - called when the "file" /dev/csync is opened in userspace
static int comsync_open (struct inode *inode, struct file *file) {
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];  // pointer to sync port driver structure

	/* Initialization check result */
	switch( sync_port->status_device ) {
	case NOT_PRESENT:
		DBGPRINTNOHEAD(DEBUG_OPENCLS, "not present\n");
		return -ENXIO;  /* No such device */
	case DEV_IN_USE:
		DBGPRINTNOHEAD(DEBUG_OPENCLS, "in use\n");
		return -EBUSY;  /* Device busy */
	case DEV_READY:
		DBGPRINTNOHEAD(DEBUG_OPENCLS, "ready\n");
		break;          /* Device ready   */
	default:
		DBGPRINTNOHEAD(DEBUG_OPENCLS, "in unknown state\n");
		return -ENXIO;  /* Unknown status */
	}

	sync_port->index = port;
	sync_port->non_block_io = 0;
	// we could do some checking on the flags supplied by "open"
	if (file->f_flags & O_NONBLOCK)
	{
		sync_port->non_block_io = 1;
	}

	init_waitqueue_head(&sync_port->rx.wait);
	init_waitqueue_head(&sync_port->tx.wait);
	sync_port->rx.wakeup = 0;
	sync_port->tx.wakeup = 0;
	sync_port->rx.head = 0;
	sync_port->rx.tail = 0;
	sync_port->tx.cur_buf_slot = sync_port->tx.buffer;
	sync_port->tx.len = 0;
	sync_port->rx.timer_rx_avail = 0;
	sync_port->tx.flow.send_xon = 0;
	sync_port->tx.flow.send_xoff = 0;
	sync_port->tx.flow.sholding = 0;
	sync_port->tx.flow.restart = 0;
	sync_port->tx.flow.rts = 0;
	sync_port->tx.flow.dtr = 0;
	sync_port->rx.timer_last_char_strobe = sync_port->rx.last_char_strobe = jiffies;
	sync_port->rx.holding = 0;
	spin_lock_init(&sync_port->driver_lock);
	sync_port->events = 0;
	sync_port->rx.dma_count = 0;
	sync_port->rx.dma_buf_cur_index = 0;	
	sync_port->tx.dma_addr_buf_index = 0;
	sync_port->tx.tx_int_wait = 0;
	sync_port->rx.sdlc_frames.tail = 0;
	sync_port->rx.sdlc_frames.head = 0;
	

	if(!sync_port->flag_escc) {
		DO_CCAR_CMD(sync_port->iusc, PURGE_RX_FIFO, &sync_port->api);
		DO_CCAR_CMD(sync_port->iusc, PURGE_TX_FIFO, &sync_port->api);
	}

	if(sync_port->flag_escc)
	{
		sync_port->escc.read_dma_busy = 0;
		sync_port->escc.write_dma_busy = 0;
		sync_port->escc.use_dma_write = 0;
		sync_port->escc.use_dma_read = 0;
		sync_port->escc.use_dma_write = 0;
		sync_port->escc.use_dma_read = 0;
	}		

	/* Successful return point */
	sync_port->status_device = DEV_IN_USE;

	//printk("comsync_open\n");

	return 0;
}

// close function - called when the "file" /dev/csync is closed in userspace
static int comsync_release (struct inode *inode, struct file *file) {
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];  // pointer to sync port driver structure


	if(!sync_port->flag_escc)
		iusc_reset(port, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api);
	else
		reset_escc_ch(sync_port);

	if(sync_port->escc.use_dma_read && !sync_port->escc.flag_comsync_III) {
		escc_dma_cancel(sync_port->escc.read_dma_ch);
		free_dma(sync_port->escc.read_dma_ch);
	}
	if(sync_port->escc.use_dma_write && !sync_port->escc.flag_comsync_III){
		escc_dma_cancel(sync_port->escc.write_dma_ch);
		free_dma(sync_port->escc.write_dma_ch);
	}
	if(sync_port->escc.flag_comsync_III)
	{
		stop_dma_dtr_req(&sync_port->escc, &sync_port->api);	
		stop_dma_wreq_read(&sync_port->escc, &sync_port->api);		
		sync_port->api.write_reg_char (sync_port->escc.base_address + 0x80, sync_port->escc.index ? 0x0c : 0x03);		
	}

	/* Successful return point */
	sync_port->status_device = DEV_READY;
	//printk("comsync_release\n");
	return 0;
}


void rx_char_scoop_timer(
	unsigned long  dummy
	)
/*++

Routine Description:

    this routine is a timer routine that will run every 10 ms and look for data in receive fifo
    that does not trigger interrupt if received data is less than threshold size

Arguments:

    None, dummy.

Return Value:

    None.

--*/
{
	int                           i, j;
	struct sync_port_drv_struct*  sync_port;
	unsigned long                 saved_jiffies = jiffies;
	unsigned int                  timer_launch = 0;
	unsigned int                  rx_avail=0;
	unsigned long flags;



	for (i=0; i<num_comsync_board ; i++)
	{
		for(j = 0; j < NUM_COMSYNC_PORT; j++) {

			sync_port = &bus_driver[i]->sync_port_driver[j];

			if (sync_port->status_device == DEV_IN_USE) {
				if ((sync_port->port_settings.smode.rx == SMODE_ASYNC) || (sync_port->port_settings.smode.rx == SMODE_ISO)) {					
					if (sync_port->rx.last_char_strobe) {
						if ((saved_jiffies - sync_port->rx.last_char_strobe) >= sync_port->fifo_ticks) {

							if(!sync_port->flag_escc)
								rx_avail = iusc_get_rx_avail((void*)sync_port->iusc, (void*)(&sync_port->api));
							else if(!sync_port->escc.use_dma_read){
								if(sync_port->api.read_reg_char(sync_port->escc.reg_address+REG0) & RX_AVAIL_ESCC)
									rx_avail = 1;									
								}

							// the check here to make sure that the characters are really stuck in fifo
							// 1. the last charater read from ISR will update both last_char_strobe and timer_last_char_strobe
							//    if they are not same in timer routine that means after the last timer run ISR has not received any character
							// 2. two successive timer read on fifo count must be same
							if ((sync_port->rx.last_char_strobe != sync_port->rx.timer_last_char_strobe) &&
								(rx_avail == sync_port->rx.timer_rx_avail) &&
								rx_avail)
							{
								if(!sync_port->flag_escc)
									iusc_fake_serial_int((void*)sync_port->iusc, &sync_port->api, RD_IE);
								else if(!sync_port->escc.use_dma_read)
								{
									#if (LINUX_VERSION_CODE > 132608)
									spin_lock_irqsave(&sync_port->driver_lock, flags);
									#else
									save_flags(flags); cli();
									#endif									
									rx_resume(sync_port, 0);

									#if (LINUX_VERSION_CODE > 132608)
									spin_unlock_irqrestore(&sync_port->driver_lock, flags);
									#else
									restore_flags(flags);
									#endif
								}


							} else {
								sync_port->rx.timer_last_char_strobe = saved_jiffies;
								sync_port->rx.timer_rx_avail = rx_avail;
							}
						}
					}

					if (!timer_launch) {
						timer_launch = 1;
					}
				}
			}
		}
	}

	if (!timer_launch)
		async_rx_timer = 0;
	else
		mod_timer(&sync_timer, jiffies + HZ/100);

}



void rx_push_data(
	struct sync_port_drv_struct*  sync_port,
	unsigned char*                data
	)
/*++

Routine Description:

	This routine will push sync data from rx FIFO to driver buffer.

Arguments:

	sync_port - points to port driver structure.

	data - pointer to data byte


Return Value:

	None.

--*/
{
	struct rx_struct*     rx = &(sync_port->rx);
	unsigned long         slots_free;

	slots_free = CIRC_BUF_EMPTY_AVAIL(
		rx->head,
		rx->tail,
		BUF_SIZE);
	
	if (slots_free)
	{
		*(rx->buffer + rx->head) =  *data;
		rx->head++;

		if ((slots_free + 1) < RX_BUF_THRESHOLD) {
			sync_port->events |= EVENT_RX_BUF_LOW;
			// handle flow control
			if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
				// software flow control
				if ((sync_port->port_settings.async_settings.sflow == 'r') ||
					(sync_port->port_settings.async_settings.sflow == 'b'))
				{
					// we don't want to send multiple xoff
					if (!sync_port->rx.holding) {
						sync_port->tx.flow.send_xoff = 1;
						sync_port->rx.holding = 1;
						if(!sync_port->flag_escc)
							iusc_fake_serial_int((void*)sync_port->iusc, &sync_port->api, TD_IE);

					}
				}
				// hardware flow control
				if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS) {
					// turn off rts so that other end stops transmission
					if (!sync_port->rx.holding) {
						sync_port->rx.holding = 1;
						if(!sync_port->flag_escc)
							iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 0);
					}
				}
			}
		}
		else
		{			
			if(sync_port->events & EVENT_RX_BUF_LOW)
				sync_port->events &= ~EVENT_RX_BUF_LOW;
		}
		if(rx->head == BUF_SIZE)
			rx->head = 0;
	}
	else
	{
		sync_port->events |= EVENT_RX_BUF_OVERRUN;		
	}
}


void rx_iusc_reg(
	struct sync_port_drv_struct*  sync_port,
	unsigned short*               rx_avail
	)
/*++

Routine Description:

	This routine will receive sync data from rx FIFO.

Arguments:

	sync_port - points to port driver structure.

	rx_avail - pointer to number of bytes available in the FIFO


Return Value:

	None.

--*/
{
	unsigned short  rx_data;
	unsigned short  rx_stat;
	unsigned char   cb_data;
	unsigned char   unlatch = 0;
	unsigned char   *data;


	do {

		if (*rx_avail >= 2) {
			SET_RICR_WORD_STATUS(sync_port->iusc, WORD_STATUS, &sync_port->api);
			rx_stat = sync_port->api.read_reg_word((void*)&sync_port->iusc->RCSR.WORD);
			rx_data = sync_port->api.read_reg_word((void*)&sync_port->iusc->RDR.WORD);
			rx_data = htons(rx_data);
			cb_data = 2;
		} else {
			SET_RICR_WORD_STATUS(sync_port->iusc, 0, &sync_port->api);
			rx_stat = sync_port->api.read_reg_word((void*)&sync_port->iusc->RCSR.WORD);
			rx_data = sync_port->api.read_reg_char((void*)&sync_port->iusc->RDR.byte.LSB);
			cb_data = 1;
		}

		if (rx_stat & RX_OVER) {
			/* overrun */
			unlatch |= RX_OVER;
			(*rx_avail) = 0;
			sync_port->events |= EVENT_OE;
			if(!sync_port->flag_escc)
				DO_CCAR_CMD(sync_port->iusc, PURGE_RX_FIFO, &sync_port->api);

		} else {
			if (rx_stat & ABORT_PE) {
				/* parity */
				unlatch |= ABORT_PE;
				sync_port->events |= EVENT_PE;
			}
			if (rx_stat & CRCE_FE) {
				/* framing */
				if (sync_port->port_settings.smode.rx == SMODE_ASYNC)
					sync_port->events |= EVENT_FE;

			}
			if (rx_stat & SHORTF_CVTYPE)
				sync_port->events |= SHORTF_CVTYPE;

			if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
				if (sync_port->port_settings.async_settings.sflow == 't' || sync_port->port_settings.async_settings.sflow == 'b') {
					if (cb_data == 2) {
						/* check MS byte */
						if ((unsigned char)(rx_data >> 8) == sync_port->port_settings.async_settings.xoff) {
							sync_port->tx.flow.sholding = 1;
							/* cancel a pending transmission restart */
							sync_port->tx.flow.restart = 0;
							cb_data--;
							(*rx_avail)--;
						} else if((unsigned char)(rx_data >> 8) == sync_port->port_settings.async_settings.xon) {
							sync_port->tx.flow.sholding = 0;
							/* queue a transmission restart */
							sync_port->tx.flow.restart = 1;
							cb_data--;
							(*rx_avail)--;
						}
						/* check LS byte */
						if ((unsigned char)rx_data == sync_port->port_settings.async_settings.xoff) {
							sync_port->tx.flow.sholding = 1;
							/* cancel a pending transmission restart */
							sync_port->tx.flow.restart = 0;
							/* if the MS byte was not a FC char, then move it into the LS byte of rx_data */
							if (cb_data == 2) {
								rx_data = rx_data >> 8;
							}
							cb_data--;
							(*rx_avail)--;
						} else if((unsigned char)rx_data == sync_port->port_settings.async_settings.xon) {
							sync_port->tx.flow.sholding = 0;
							/* queue a transmission restart */
							sync_port->tx.flow.restart = 1;
							/* if the MS byte was not a FC char, then move it into the LS byte of rx_data */
							if (cb_data == 2) {
								rx_data = rx_data >> 8;
							}
							cb_data--;
							(*rx_avail)--;
						}
					} else {
						if ((unsigned char)rx_data == sync_port->port_settings.async_settings.xoff) {
							sync_port->tx.flow.sholding = 1;
							/* cancel a pending transmission restart */
							sync_port->tx.flow.restart = 0;
							cb_data = 0;
							(*rx_avail)--;
						} else if((unsigned char)rx_data == sync_port->port_settings.async_settings.xon) {
							sync_port->tx.flow.sholding = 0;
							/* queue a transmission restart */
							sync_port->tx.flow.restart = 1;
							cb_data = 0;
							(*rx_avail)--;
						}
					}
					if (rx_stat & CRCE_FE) {
						/* CRC */
						sync_port->events |= EVENT_CRC;
					}
				}
			}

			if (cb_data) {
				data = (unsigned char*)&rx_data;
				if (rx_stat & RX_BOUND) {
					/* un-latch the status saved by the RX_BOUND
					 * this gives us access to 1st and 2nd BE
					 */
					rx_stat = sync_port->api.read_reg_word((void*)&sync_port->iusc->RCSR.WORD);

					if (cb_data == 2) {
						/* if 1st BE is set our 16-bit read of RDR only removed
						 * a single character from the RX FIFO
						 */
						if (rx_stat & FIRST_BE) {
							/* only one char was removed from the RX FIFO and
							 * only the MS byte of the read data is valid
							 * we must shift the data down one byte because the
							 * push function expects the data byte to be the
							 * LS byte
							 */
							data++;
							rx_push_data(sync_port, data);
							(*rx_avail)--;
						} else {
							/* both chars were removed from the RX FIFO
							 * and both are valid
							 */
							(*rx_avail) -= cb_data;
							while(cb_data--)
								rx_push_data(sync_port, data++);
						}
					} else {
						/* since we did a single byte read, the byte
						 * is in the LS byte of rx_data
						 */
						rx_push_data(sync_port, data);
						(*rx_avail)--;
					}
				}
				else {
					(*rx_avail) -= cb_data;
					while(cb_data--)
						rx_push_data(sync_port, data++);
				}
			}
		}
	} while (*rx_avail);

	if (unlatch) {
		/* we need to unlatch someof the handled status bits */
		sync_port->api.write_reg_char((void*)&sync_port->iusc->RCSR.byte.LSB, unlatch);
	}
}

void rx_escc_reg(struct sync_port_drv_struct*  sync_port)
{
	unsigned char ch;
	struct _api_context *api = &sync_port->api;

	
	while (api->read_reg_char(sync_port->escc.reg_address+REG0) & RX_AVAIL_ESCC)
	 {
		ch = api->read_reg_char (sync_port->escc.reg_address+REG8);
		switch(sync_port->port_settings.dbits.rx)
		{
		case RX_5:
			ch &=0x1f;
			break;
		case RX_6:
			ch &=0x3f;
			break;
		case RX_7:
			ch &=0x7f;
			break;
		}

		if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
			if (sync_port->port_settings.async_settings.sflow == 't' || sync_port->port_settings.async_settings.sflow == 'b') {
				if (ch == sync_port->port_settings.async_settings.xoff) {
					sync_port->tx.flow.sholding = 1;
					/* cancel a pending transmission restart */
					sync_port->tx.flow.restart = 0;
					continue;
				} else if(ch == sync_port->port_settings.async_settings.xon) {
					sync_port->tx.flow.sholding = 0;
					/* queue a transmission restart */
					sync_port->tx.flow.restart = 1;
					continue;
				}
			}
		}

		rx_push_data(sync_port, &ch);

	} 
}

void rx_resume(
	struct sync_port_drv_struct*  sync_port,
	unsigned short*               rx_avail
	)
{
	if(!sync_port->flag_escc)
		rx_iusc_reg(sync_port, rx_avail);
	else
		rx_escc_reg(sync_port);

	sync_port->rx.wakeup = 1;
	wake_up_interruptible(&sync_port->rx.wait);

	// handle transmission restart flow async
	if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
		if (sync_port->port_settings.async_settings.sflow == 't' || sync_port->port_settings.async_settings.sflow == 'b') {
			if (sync_port->tx.flow.restart) {
				if(!sync_port->flag_escc)
					iusc_fake_serial_int((void*)sync_port->iusc, &sync_port->api, TD_IE);

				sync_port->tx.flow.restart = 0;
			}
		}
	}
	if((sync_port->port_settings.smode.rx == SMODE_ASYNC) || (sync_port->port_settings.smode.rx == SMODE_ISO)){
		sync_port->rx.timer_last_char_strobe = sync_port->rx.last_char_strobe = jiffies;
	}
}

int comsyncIII_read(struct sync_port_drv_struct *sync_port, int count, unsigned char *crc_stat)
{	
	volatile int space = CIRC_BUF_DATA_AVAIL(sync_port->rx.sdlc_frames.head, sync_port->rx.sdlc_frames.tail, BUF_SIZE);
	
	if(space)
	{
		space = sync_port->rx.sdlc_frames.info[sync_port->rx.sdlc_frames.tail].len;	
		*crc_stat = 	sync_port->rx.sdlc_frames.info[sync_port->rx.sdlc_frames.tail].stat;
		if(count >= space)
		{
			sync_port->rx.sdlc_frames.tail++;
			if(sync_port->rx.sdlc_frames.tail >= (BUF_SIZE))
				sync_port->rx.sdlc_frames.tail = 0;
			return space;
		}
		else
			return -1;
	}
	else
	{
		return -1;
	}
}

// read function - called when the "file" /dev/csync is read in userspace
static ssize_t comsync_read (struct file *file, char *buf,
		size_t count, loff_t *ppos)
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,18,0))	
	struct inode *inode = file->f_dentry->d_inode;
	int port = ((NUM(file->f_dentry->d_inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
  int board = (NUM(file->f_dentry->d_inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#else 
	struct inode *inode = file_inode(file);
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#endif
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];  // pointer to sync port driver structure
	int ret;
	volatile int space = CIRC_BUF_DATA_AVAIL(sync_port->rx.head, sync_port->rx.tail, BUF_SIZE);
	int size;
	unsigned long         slots_free;
	int sdlc_frame_space, frame_len=0;
	unsigned char crc_stat;

	if(space && sync_port->port_settings.smode.rx == SMODE_HDLC && sync_port->escc.flag_comsync_III)
	{
		sdlc_frame_space = CIRC_BUF_DATA_AVAIL(sync_port->rx.sdlc_frames.head, sync_port->rx.sdlc_frames.tail, BUF_SIZE);			
		if(!sdlc_frame_space)
		{
			space = 0;			
		}		
		else
		{
			frame_len = sync_port->rx.sdlc_frames.info[sync_port->rx.sdlc_frames.tail].len;
			if(space < frame_len)
			{
				space = 0;
			}
		}
	}
	

	if (file->f_flags & O_NONBLOCK)
	{
		sync_port->non_block_io = 1;
	}
	// check if we have data - if not, sleep
	// wake up in interrupt_handler
	while (space == 0) {
		if(sync_port->non_block_io == 0)
		{
			ret = wait_event_interruptible(sync_port->rx.wait, sync_port->rx.wakeup);
			sync_port->rx.wakeup = 0;
			if (ret)
				return ret;
		}
		else
			return 0;

		space = CIRC_BUF_DATA_AVAIL(sync_port->rx.head, sync_port->rx.tail, BUF_SIZE);
		
		if(space && sync_port->port_settings.smode.rx == SMODE_HDLC && sync_port->escc.flag_comsync_III)
		{
			sdlc_frame_space = CIRC_BUF_DATA_AVAIL(sync_port->rx.sdlc_frames.head, sync_port->rx.sdlc_frames.tail, BUF_SIZE);		
			
			if(!sdlc_frame_space)
			{
				space = 0;
				continue;
			}
			space = CIRC_BUF_DATA_AVAIL(sync_port->rx.head, sync_port->rx.tail, BUF_SIZE);
			frame_len = sync_port->rx.sdlc_frames.info[sync_port->rx.sdlc_frames.tail].len;
			if(space < frame_len)
			{
				space = 0;
			}	
			
		}
	}

	if(count <= space)
		space = count;
	
	if(space && sync_port->port_settings.smode.rx == SMODE_HDLC && sync_port->escc.flag_comsync_III)
	{
		space = comsyncIII_read(sync_port, space, &crc_stat);			
		if(space < 0)
			return -1;		
	}

	if((sync_port->rx.tail + space) >= BUF_SIZE)
	{
		size = (BUF_SIZE) - sync_port->rx.tail;

		ret = copy_to_user(buf, sync_port->rx.buffer + sync_port->rx.tail, size);
		if (ret != 0)
			return -EFAULT;

		buf += size;
		size = space - size;

		ret = copy_to_user(buf, sync_port->rx.buffer, size);
		if (ret != 0)
			return -EFAULT;
		buf += size;
		sync_port->rx.tail = size;
	}
	else
	{
		ret = copy_to_user(buf, sync_port->rx.buffer + sync_port->rx.tail, space);
		if (ret != 0)
			return -EFAULT;
		sync_port->rx.tail+=space;
		if(sync_port->rx.tail >= (BUF_SIZE))
			sync_port->rx.tail = 0;
		buf += space;
	}
	
	if(sync_port->port_settings.last_char_crc_stat && sync_port->port_settings.smode.rx == SMODE_HDLC && sync_port->escc.flag_comsync_III)
	{		
		if(count >= space + 1)
		{			
			ret = copy_to_user(buf, &crc_stat, 1);
			if (ret != 0)
				return -EFAULT;	
			space++;		
		}
	}

	if(sync_port->port_settings.smode.rx == SMODE_ASYNC) {
		if((sync_port->port_settings.async_settings.sflow == 'r') ||
			(sync_port->port_settings.async_settings.sflow == 'b'))
		{
			if(sync_port->rx.holding) {
				slots_free = CIRC_BUF_EMPTY_AVAIL(
							sync_port->rx.head,
							sync_port->rx.tail,
							BUF_SIZE);

				if (slots_free > RX_BUF_THRESHOLD) {
					sync_port->rx.holding = 0;
					sync_port->tx.flow.send_xon = 1;

					if(!sync_port->flag_escc)
						iusc_fake_serial_int((void*)sync_port->iusc, &sync_port->api, TD_IE);


				}
			}
		}
		if(sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS) {
			// turn on rts so that other end starts transmission
			slots_free = CIRC_BUF_EMPTY_AVAIL(
							sync_port->rx.head,
							sync_port->rx.tail,
							BUF_SIZE);
			if(sync_port->rx.holding) {
				if (slots_free > RX_BUF_THRESHOLD) {
					sync_port->rx.holding = 0;
					iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 1);
				}
			}
		}
	}

	return space;
}

void handle_async_tx_flow(struct sync_port_drv_struct *sync_port, unsigned short *tx_avail)
/*++

Routine Description:

    this routine handles flow control for async data

Arguments:

    tx_avail - pointer number of tx chars empty in fifo

    sync_port - points to port driver structure.

Return Value:

    None.

--*/
{

	if(sync_port->tx.flow.send_xoff) {
		sync_port->tx.flow.send_xoff = 0;
		if(!sync_port->flag_escc)
			sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, sync_port->port_settings.async_settings.xoff);
		else
			tx_escc(sync_port, &sync_port->port_settings.async_settings.xoff, 1);
		(*tx_avail)--;

	} else if(sync_port->tx.flow.send_xon) {
		sync_port->tx.flow.send_xon = 0;
		if(!sync_port->flag_escc)
			sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, sync_port->port_settings.async_settings.xon);
		else
			tx_escc(sync_port, &sync_port->port_settings.async_settings.xon, 1);
		(*tx_avail)--;
	}
}


static inline void copy_to_iusc_escc_tx(
	struct sync_port_drv_struct *sync_port,
	unsigned short       len,
	unsigned char        is_eof
	)
/*++

Routine Description:

    copy buffer of data into IUSC transmit FIFO

Arguments:

    sync_port - points to port driver structure.

    len - length of buffer

    is_eof - flag to issue EOF command

Return Value:

    None.

--*/
{
	DBGPRINT(DEBUG_WRITE, KERN_INFO,
		"%s(%p, %u, %u)\n",
		__FUNCTION__, sync_port->iusc, len, is_eof);

	if(sync_port->flag_escc)
	{		
		while(len--) {
			while(!(sync_port->api.read_reg_char (sync_port->escc.reg_address+REG0) & 0x04));
			sync_port->api.write_reg_char (sync_port->escc.reg_address+REG8, *sync_port->tx.cur_buf_slot);
			sync_port->tx.cur_buf_slot++;
		}		
		return;
	}
	//else IUSC

	/* Put data into the Transmitter FIFO here.
	 * Remember that the data can be written to the TX Fifo
	 * in 16 bit quantities (unsigned short), BUT if the number
	 * of bytes is odd, the last byte must be written to the LSB
	 * of the TDR register.
	 */

	/* If the buffer address is on an odd byte boundary then we will
	 * do a single byte write first to get ourselves aligned for the
	 * remaining data.  This is to gain maximum performance.
	 */
	if(((unsigned long)sync_port->tx.cur_buf_slot) % 2) {
		if((len == 1) && is_eof) {
			sync_port->api.write_reg_word((void*)&sync_port->iusc->TCSR.WORD, TCSR_ISSUE_TCMD(sync_port->iusc->TCSR.WORD, &sync_port->api) | SET_EOFM);
		}
		sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, *sync_port->tx.cur_buf_slot);
		sync_port->tx.cur_buf_slot++;
		len--;
	}

	/* Transmit the remaining whole 16-bit data. */
	while(len >= 2) {
		if((len == 2) && is_eof) {
			sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, *sync_port->tx.cur_buf_slot);
			sync_port->tx.cur_buf_slot++;
			sync_port->api.write_reg_word((void*)&sync_port->iusc->TCSR.WORD, TCSR_ISSUE_TCMD(sync_port->iusc->TCSR.WORD, &sync_port->api) | SET_EOFM);
			sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, *sync_port->tx.cur_buf_slot);
			sync_port->tx.cur_buf_slot++;
		} else {
			sync_port->api.write_reg_word((void*)&sync_port->iusc->TDR.WORD, htons(*((unsigned short*)sync_port->tx.cur_buf_slot)));
			sync_port->tx.cur_buf_slot+=2;
		}
		len -= 2;
	}

	/* Now transmit any remaining 8-bit data. */
	if(len) {
		if(is_eof) {
			sync_port->api.write_reg_word((void*)&sync_port->iusc->TCSR.WORD, TCSR_ISSUE_TCMD(sync_port->iusc->TCSR.WORD, &sync_port->api) | SET_EOFM);
		}
		sync_port->api.write_reg_char((void*)&sync_port->iusc->TDR.byte.LSB, *sync_port->tx.cur_buf_slot);
		sync_port->tx.cur_buf_slot++;
	}
}


void tx_resume(struct sync_port_drv_struct*  sync_port, unsigned short *fifo_avail)
/*++

Routine Description:

    copy buffer of data into IUSC transmit FIFO

Arguments:

    context - points to port sync_port_drv_struct structure.

    fifo_avail - pointer to number of tx chars empty in fifo

Return Value:

    None.

--*/
{
	unsigned int space;
	int eof = 0;
	struct _api_context *api = &sync_port->api;


	DBGPRINT(DEBUG_WRITE, KERN_INFO,"%s len %ld *fifo_avail %d\n",__FUNCTION__, sync_port->tx.len, *fifo_avail);
	if(!*fifo_avail)
	{
		return;
	}

	if (sync_port->port_settings.smode.tx == SMODE_ASYNC)
		handle_async_tx_flow(sync_port, fifo_avail);

	if((sync_port->tx.len == 0) || (sync_port->tx.flow.sholding))
		return;

	space = sync_port->tx.len;
	if(space > *fifo_avail)
		space = *fifo_avail;

	if (sync_port->port_settings.smode.rx == SMODE_HDLC)
		if(!(sync_port->tx.len - space))
			eof = 1;
	/* resume moving data from slots to TX fifo */
	copy_to_iusc_escc_tx(sync_port, space, eof);


	if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE) {
		/* unlatch the IDLE_SENT bit */
		if(!sync_port->flag_escc)
			sync_port->api.write_reg_char((void*)&sync_port->iusc->TCSR.byte.LSB, sync_port->api.read_reg_char((void *)&sync_port->iusc->TCSR.byte.LSB)|IDLE_SENT);

	}

	sync_port->tx.len -= space;

	if (sync_port->tx.len == 0) {
		if(sync_port->flag_escc) {
			if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE)
			{
				sync_port->escc.WriteReg.r5.wr5.bf.rts = 0;
				api->write_reg_char(sync_port->escc.reg_address+REG5,sync_port->escc.WriteReg.r5.wr5.reg);
			}
		}

		sync_port->tx.wakeup = 1;
		sync_port->tx.cur_buf_slot = sync_port->tx.buffer;
		wake_up_interruptible(&sync_port->tx.wait);
	}

}

// write function - called when the "file" /dev/csync is written in userspace
static ssize_t comsync_write (struct file *file, const char *buf,
		size_t count, loff_t *ppos)
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,18,0))	
	struct inode *inode = file->f_dentry->d_inode;
	int port = ((NUM(file->f_dentry->d_inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
  int board = (NUM(file->f_dentry->d_inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#else 
	struct inode *inode = file_inode(file);
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#endif
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];  // pointer to sync port driver structure
	int ret;
	unsigned long dma_stat;
	unsigned long   flags;	
	
	// we could do some checking on the flags supplied by "open"
	if (file->f_flags & O_NONBLOCK)
	{
		sync_port->non_block_io = 1;
	}

	if(count > BUF_SIZE)
		count = BUF_SIZE;

	// check if we have data - if not, sleep
	// wake up in interrupt_handler
	while (sync_port->tx.len) {
		if(sync_port->non_block_io == 0)
		{
			ret = wait_event_interruptible(sync_port->tx.wait, sync_port->tx.wakeup);
			sync_port->tx.wakeup = 0;
			if (ret)
				return ret;
		}
		else
			return 0;
	}
	
	if(!sync_port->escc.flag_comsync_III)
		sync_port->tx.len = count;

	/* if we are framing the TX data with RTS, then ensure RTS is
	 * on before putting the data in the FIFO
	 */
	if(!sync_port->flag_escc) {
		if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE) {
			iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 1);
		}
	}
	
	if(!sync_port->escc.flag_comsync_III)
	{
		ret = copy_from_user(sync_port->tx.buffer, buf, count);
		if (ret != 0)
			return -EFAULT;
	}
		
	if(sync_port->escc.flag_comsync_III)
	{		
		if(count > DMA_BUF_REQEST)
			count = DMA_BUF_REQEST;		
		ret = copy_from_user(sync_port->tx.dma_buffer[sync_port->tx.dma_addr_buf_index], buf, count);
		if (ret != 0)
			return -EFAULT;				
		dma_stat = sync_port->api.read_reg_long (sync_port->escc.base_address + 0x84);		
		
		#if (LINUX_VERSION_CODE > 132608)
		spin_lock_irqsave(&sync_port->driver_lock, flags);
		#else
		save_flags(flags); cli();
		#endif		
		// now set the count value, otherwise it'll race with ISR
		sync_port->tx.len = count;													
		
		if(dma_stat & TX_BUFS_BUSY(sync_port->escc.index)) 
		{				
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif

			ret = wait_event_interruptible(sync_port->tx.wait, sync_port->tx.wakeup);
			sync_port->tx.wakeup = 0;
			if (ret)
				return ret;					
		}				
		else
		{	
			if(sync_port->port_settings.smode.rx == SMODE_HDLC)
			{								
				if(!sync_port->tx.tx_int_wait)
				{
					comsyncIII_transmit_dma(sync_port);				
				}
			}
			else
			{
				comsyncIII_transmit_dma(sync_port);
			}
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif
		}
	}
	else if (!sync_port->tx.flow.sholding) {
		if(!sync_port->flag_escc)
			iusc_fake_serial_int((void*)sync_port->iusc, &sync_port->api, TD_IE);
		else if(sync_port->escc.use_dma_write && !sync_port->escc.flag_comsync_III)
			start_DMA_write(sync_port);
		else
			escc_tx_fifo(sync_port);
	}

	return count;
}


static unsigned long get_fifo_ticks(
	struct sync_port_drv_struct*  sync_port
	)
/*++

Routine Description:

    this routine returns the minimum number of ticks to wait for port
    settings (bps, parity etc).

Arguments:

    sync_port - points to port driver structure.

Return Value:

    number of ticks.

--*/
{
	// total number of fifo bits considering max width
	unsigned long fifo_bits;
	unsigned long fifo_ticks;

	// 1 stop bit + 1 start bit + 2 parity bit
	fifo_bits = (sync_port->port_settings.dbits.rx + 1 + 1 + 2) * FIFO_SIZE;
	fifo_ticks = (fifo_bits * HZ) / sync_port->port_settings.rxclk.bps;

	if (!fifo_ticks)
		fifo_ticks = 1;

	return fifo_ticks;
}




static int pset_changed(
	struct sync_port_drv_struct*  sync_port
	)
/*++

Routine Description:

    this routine does post port setup processing like interrupt setup etc.

Arguments:

    sync_port - points to port driver structure.

Return Value:

	returns 0 upon success, otherwise returns non-zero

--*/
{
	int             ret = 0;
	unsigned long   flags;
	iusc_int_setup  int_setup;

	/* give the IUSC a purge */
	DBGPRINT(DEBUG_PSET, KERN_INFO,
		"[%u] purging IUSC\n", sync_port->index);
	if(!sync_port->flag_escc)
		DO_CCAR_CMD(sync_port->iusc, PURGE_RXTX_FIFO, &sync_port->api);

	if (!async_rx_timer && !sync_port->escc.flag_comsync_III) {
		if ((sync_port->port_settings.smode.rx == SMODE_ASYNC) || (sync_port->port_settings.smode.rx == SMODE_ISO)) {
			DBGPRINT(DEBUG_PSET, KERN_INFO,
				"[%u] starting async timer\n", sync_port->index);
			sync_port->fifo_ticks = get_fifo_ticks(sync_port);
			#if (LINUX_VERSION_CODE > 132608)
			spin_lock_irqsave(&sync_port->driver_lock, flags);
			#else
			save_flags(flags); cli();
			#endif
			async_rx_timer = 1;
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif
			mod_timer(&sync_timer, jiffies + HZ/100);
		}
	}


	if(sync_port->port_settings.smode.rx == SMODE_ASYNC) {
		/* hardware flow control */
		if(sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS) {
			if(!sync_port->rx.holding) {
				sync_port->rx.holding = 1;
				iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 1);
			}
		}
	}

	memset(&int_setup, 0, sizeof(int_setup));

	int_setup.port = sync_port->port_settings.port;
	int_setup.rx_int_level = 24;
	int_setup.tx_int_level = 24;

	if(sync_port->port_settings.smode.rx == SMODE_HDLC) {
		/* HDLC */

		int_setup.rx_status_ia = EXITED_HUNT_IA | IDLE_RECV_IA;
	} else if (sync_port->port_settings.smode.rx == SMODE_MONO) {
		/* not ASYNC and not HDLC */
		// currently doing for mono
		// It seems that if there is idle char received and if IDLE_RECV_IA interrupt is not armed then it does not generate
		// interrupts for other rx reasons, a bug in IUSC. It's better to enable all possible interrupts
		// so that IUSC does not deadlock on a condition for which interrupt is not armed
		int_setup.rx_status_ia = EXITED_HUNT_IA | IDLE_RECV_IA | BREAK_ABORT_RECV_IA;
	} else {
		/* ASYNC or ISO*/
		int_setup.io_misc_ia = DCD_IA | RXR_IA | TXR_IA;
		if(!(sync_port->port_settings.async_settings.hflow & LSET_HFLOW_CTS))
			int_setup.io_misc_ia |= CTS_IA;
		int_setup.rx_status_ia = BREAK_ABORT_RECV_IA;
	}

	if(sync_port->port_settings.smode.tx != SMODE_ASYNC) {
		int_setup.tx_status_ia = TICR_ALL_IA & ~IDLE_SENT_IA;
	}

	DBGPRINT(DEBUG_PSET | DEBUG_INTR, KERN_INFO,
		"[%u] setting up IUSC interrupts "
		"(%d,0x%02x,%u,0x%02x,%u,0x%04x,0x%02x,0x%02x)\n",
		int_setup.port, int_setup.port, int_setup.rx_status_ia,
		int_setup.rx_int_level, int_setup.tx_status_ia,
		int_setup.tx_int_level, int_setup.io_misc_ia,
		int_setup.rx_dma_ia, int_setup.tx_dma_ia);

	ret = iusc_interrupt_setup(&int_setup, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api);
	if(ret) {
		PRINT(KERN_ERR, "[%u] failed to setup IUSC interrupts!\n",
			int_setup.port);
	} else {
		DBGPRINT(DEBUG_PSET | DEBUG_INTR, KERN_INFO,
			"[%u] enabling IUSC master serial interrupt\n",
			int_setup.port);
		iusc_set_serial_mie((void*)sync_port->iusc, &sync_port->api, 1);

	}

	if((sync_port->port_settings.line_mode & 0x7) == LM_SHUTDOWN) {
		PRINT(KERN_WARNING, "[%u] warning, the line interface has been set "
			"to shutdown\n", int_setup.port);
	}
		
	return ret;
}

static int apply_pset(
	struct sync_port_drv_struct*  sync_port,
	BHN_PortSettings*             new_pset
	)
/*++

Routine Description:

    this routine does port setup processing

Arguments:

    sync_port - points to port driver structure.

    new_pset  - pointer to BHN_PortSettings structure

Return Value:

	returns 0 upon success, otherwise returns non-zero

--*/
{
	int  ret;
	int  result;


	iusc_reset(new_pset->port, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api);

	/* attempt to configure the IUSC with the new settings
	 * this will disable the interrupt going to the coldfire
	 */
	ret = iusc_port_setup(new_pset, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api, 0, 0);
	if(ret) {
		PRINT(KERN_WARNING,
			"new port settings structure should have been "
			"previously validated (0x%x)!\n", ret);

		/* try and put back our old settings */
		iusc_reset(new_pset->port, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api);
		result = iusc_port_setup(&sync_port->port_settings, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api, 0, 0);
		if(result) {
			/* now we're in real trouble */
			PRINT(KERN_ERR,
				"could not restore previously valid port settings, "
				"leaving interrupts disabled (0x%x)!\n",
				result);
		} else {
			result = pset_changed(sync_port);
			if(result) {
				PRINT(KERN_ERR,
					"failed to handle changed settings event (0x%x)!\n",
					result);
			}
		}
	} else {
		/* the new settings were successfully applied */
		memcpy(&(sync_port->port_settings), new_pset,
			sizeof(sync_port->port_settings));

		ret = pset_changed(sync_port);
		if(ret) {
			PRINT(KERN_ERR,
				"failed to handle changed settings event (0x%x)!\n",
				ret);
		}
	}

	return ret;
}



// ioctl - I/O control

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))		
static long comsync_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg)
#else
static int comsync_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
#endif
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,18,0))	
	struct inode *inode = file->f_dentry->d_inode;
	int port = ((NUM(file->f_dentry->d_inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
  int board = (NUM(file->f_dentry->d_inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))	
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#else 
	struct inode *inode = file_inode(file);
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#endif
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];  // pointer to sync port driver structure
	unsigned long ret;
	int retval = 0;
	unsigned long flags;
	int i;
	
	switch ( cmd ) {

	case TIOHUNT:
		if(sync_port->port_settings.smode.rx) {
			if(!sync_port->flag_escc) {
				sync_port->api.write_reg_word((void*)&sync_port->iusc->RCSR.WORD, ENTER_HUNT);
			}
			else 
			{							
				#if (LINUX_VERSION_CODE > 132608)
				spin_lock_irqsave(&sync_port->driver_lock, flags);
				#else
				save_flags(flags); cli();
				#endif
				
				sync_port->escc.hunt_mode = 1;
				sync_port->escc.WriteReg.r3.wr3.bf.enter_hunt = 1;
				sync_port->api.write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
				if(!sync_port->escc.flag_comsync_III) 
				{
					//reinitialize DMA
					sync_port->api.write_reg_char (sync_port->escc.base_address + 0x80, sync_port->escc.index ? 0x08 : 0x02);		
					sync_port->rx.dma_count = DMA_BUF_REQEST;
					sync_port->rx.dma_buf_cur_index = 0;
					sync_port->rx.dma_buf_index = 0;
					// loading receive dma list addresses										
					for(i=0;i<NUM_DMA_LIST;i++)
					{
						sync_port->api.write_reg_long ((void __iomem *)sync_port->rx.dma_list_addr[i], sync_port->rx.dma_phy_addr[i]);
						sync_port->api.write_reg_long ((void __iomem *)sync_port->rx.dma_count_addr[i], DMA_BUF_REQEST);
					}	
					sync_port->rx.head = 0;
					sync_port->rx.tail = 0;
				}
				#if (LINUX_VERSION_CODE > 132608)
				spin_unlock_irqrestore(&sync_port->driver_lock, flags);
				#else
				restore_flags(flags);
				#endif				
			}
		}
		break;

	case TIOGETPORTSET:
	{
		BHN_PortSettings *settings;

		settings = (BHN_PortSettings *)arg;
		memcpy(settings, &sync_port->port_settings, sizeof(BHN_PortSettings));
	}
	break;

	case TIOSETPORTSET:
	{
		BHN_PortSettings  pset;
		unsigned long val;
		int i;
		
		ret = copy_from_user((void*)&pset, (void*)arg, sizeof(pset));		
		if (ret) {
			PRINT(KERN_ERR,
				"TIOSETPORTSET failed to copy port settings "
				"from user(arg=0x%lx ret=%lu)\n",
				arg, ret);
			retval = -EFAULT;
		} else {
			/* override the port member to be sure */
			pset.port = port;
			if(!sync_port->flag_escc) {
				/* validate the new settings */
				retval = iusc_port_setup(&pset, (void*)sync_port->iusc, (void*)sync_port->pld, &sync_port->api, 0, 1);
				if(retval) {
					PRINT(KERN_INFO,
						"TIOSETPORTSET failed port settings validation (0x%x)\n",
						retval);
				} else {
					retval = apply_pset(sync_port, &pset);										
					if(pset.lloop == 1 && !retval)
					{					
						printk("loopback %d\n",pset.port);
						iusc_setup_loopback ((void*)sync_port->iusc, &sync_port->api, INTERNAL_LOCAL);						
					}
				}
			}
			else {
				//flow control not supported
				pset.async_settings.sflow = 0;
				// MIE of ESCC is global for both ports, therefore MIE value should be copied if is set
				// otherwise a running port will be disabled due to reset by another port
				if(bus_driver[board]->flag_escc_mie)
					sync_port->escc.WriteReg.r9.wr9.bf.mie = 1;

				/*********************************************************
				port 0 Tx = DMA channel 0
				port 1 Rx = DMA channel 1
				port 2 Tx = DMA channel 2
				port 3 Rx = DMA channel 3
				**********************************************************/

				if(!pset.disable_dma_write) {
					if(sync_port->escc.flag_comsync_III)
					{
						sync_port->escc.use_dma_write = 1;
					}
					else
					{
						if (! (ret = request_dma(port*2, COMSYNC_DRV_NAME)) ) {
							sync_port->escc.use_dma_write = 1;
							sync_port->escc.write_dma_ch = port*2;
						}
						else
						{
							sync_port->escc.use_dma_write = 0;
						}
					}
				}
				if(!pset.disable_dma_read) {
					if(sync_port->escc.flag_comsync_III)
					{
						sync_port->escc.use_dma_read = 1;
					}
					else
					{
						if (! (ret = request_dma(port*2 + 1, COMSYNC_DRV_NAME)) ) {
							sync_port->escc.use_dma_read = 1;
							sync_port->escc.read_dma_ch = port*2 + 1;
						}
						else
						{
							sync_port->escc.use_dma_read = 0;
						}
					}
				}
				memcpy(&(sync_port->port_settings), &pset, sizeof(sync_port->port_settings));
				retval = setup_escc(&pset, &sync_port->escc, &sync_port->api);
				if(retval)
				{
					reset_escc_ch(sync_port);
					break;
				}
				
				if(sync_port->escc.flag_comsync_III)
				{
					if(!sync_port->escc.index)
						sync_port->api.write_reg_char (sync_port->escc.base_address + 0x80, 0x03);
					else
						sync_port->api.write_reg_char (sync_port->escc.base_address + 0x80, 0x0c);		
					start_dma_wreq_read(&sync_port->escc, &sync_port->api);
					start_dma_dtr_req(&sync_port->escc, &sync_port->api);
									
					val = (32 * 1000000)/sync_port->port_settings.txclk.bps;					
					if(val > 4095) val = 4095;
					
					// set TX Throttle
					sync_port->api.write_reg_long (sync_port->escc.base_address + 0x8c, val);
					
					val = (40 * 1000000) / sync_port->port_settings.rxclk.bps;
					if(val > 4095) val = 4095;
					
					// set RX timeout
					sync_port->api.write_reg_long (sync_port->escc.base_address + 0x94, val);	
					
					if(sync_port->port_settings.smode.rx != SMODE_ASYNC && !sync_port->escc.hunt_mode)
					{
						sync_port->escc.hunt_mode = 1;
						sync_port->escc.WriteReg.r3.wr3.bf.enter_hunt = 1;
						sync_port->api.write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
					}				
					
					sync_port->api.write_reg_long (sync_port->escc.base_address + 0xa4, RELEASE_RX_HALT(sync_port->escc.index));
					sync_port->rx.dma_count = DMA_BUF_REQEST;
					// loading receive dma list addresses										
					for(i=0;i<NUM_DMA_LIST;i++)
					{
						sync_port->api.write_reg_long ((void __iomem *)sync_port->rx.dma_list_addr[i], sync_port->rx.dma_phy_addr[i]);
						sync_port->api.write_reg_long ((void __iomem *)sync_port->rx.dma_count_addr[i], DMA_BUF_REQEST);
					}										
					
					sync_port->rx.dma_buf_index = 0;
					sync_port->tx.dma_buf_index = 0;			
					if(!sync_port->escc.WriteReg.r5.wr5.bf.tx_enab)
					{
						sync_port->escc.WriteReg.r5.wr5.bf.tx_enab = 1;
						sync_port->api.write_reg_char (sync_port->escc.reg_address+REG5, sync_port->escc.WriteReg.r5.wr5.reg);
					}	
			
				}								
				else
				{				
					sync_port->fifo_ticks = get_fifo_ticks(sync_port);
					#if (LINUX_VERSION_CODE > 132608)
					spin_lock_irqsave(&sync_port->driver_lock, flags);
					#else
					save_flags(flags); cli();
					#endif
					async_rx_timer = 1;
					#if (LINUX_VERSION_CODE > 132608)
					spin_unlock_irqrestore(&sync_port->driver_lock, flags);
					#else
					restore_flags(flags);
					#endif
					mod_timer(&sync_timer, jiffies + HZ/100);
					if(sync_port->escc.WriteReg.r9.wr9.bf.mie)
						bus_driver[board]->flag_escc_mie = 1;
				}
				
				if(pset.lloop)															
				{
					sync_port->escc.WriteReg.r14.wr14.bf.local_loopback = 1;
				}
			}
		}
	}
	break;

	case TIOLOOPBACK:
		if(!sync_port->flag_escc)
			iusc_setup_loopback ((void*)sync_port->iusc, &sync_port->api, INTERNAL_LOCAL);
		else
		{
			sync_port->escc.WriteReg.r14.wr14.bf.local_loopback = 1;
		}
		break;

	case TIOGETEVENT:
		ret = copy_to_user((void*)arg, (void*)&(sync_port->events), sizeof(sync_port->events));
		if (ret) {
			PRINT(KERN_ERR, "TIOGETEVENT failed to copy write info to user "
				"(arg=0x%lx ret=%lu)\n", arg, ret);
			retval = -EFAULT;
		}
		sync_port->events = 0;//reset events
		break;

	case TIOSETCLRBRK:
	{
		int modem_sig = 0;

		ret = copy_from_user((void*)&modem_sig, (void*)arg, sizeof(int));
		if (ret) {
			PRINT(KERN_ERR, "TIOCMSET failed to copy write info from user "
				"(arg=0x%lx ret=%lu)\n", arg, ret);
			retval = -1;
		} else {
			if(modem_sig) {
				if(!sync_port->flag_escc)
					iusc_send_break((void*)sync_port->iusc, &sync_port->api, 1);
				else
					escc_send_break(&sync_port->escc, &sync_port->api, 1);
			}
			else
			{
				if(!sync_port->flag_escc)
					iusc_send_break((void*)sync_port->iusc, &sync_port->api, 0);
				else
					escc_send_break(&sync_port->escc, &sync_port->api, 0);
			}

		}
		break;
	}

	case TIOCSERGETMSR:
	{
		unsigned char modem_sig = 0;
		if(!sync_port->flag_escc)
		{
			if(iusc_get_cts ((void*)sync_port->iusc, &sync_port->api))
				modem_sig |= 0x10;
		}
		else
		{
			if(sync_port->api.read_reg_char (sync_port->escc.reg_address+REG0) &  CTS)
				modem_sig |= 0x10;
		}
		if(!sync_port->flag_escc)
		{
			if(iusc_get_dcd ((void*)sync_port->iusc, &sync_port->api))
				modem_sig |= 0x80;
		}
		else
		{
			if(sync_port->api.read_reg_char (sync_port->escc.reg_address+REG0) &  DCD)
				modem_sig |= 0x80;
		}
		if(!sync_port->flag_escc)
		{
			if(iusc_get_dsr ((void*)sync_port->iusc, &sync_port->api))
				modem_sig |= 0x20;
		}

		if(!sync_port->flag_escc)
		{
			if(iusc_get_ri ((void*)sync_port->iusc, &sync_port->api))
				modem_sig |= 0x40;
		}

		ret = copy_to_user((void*)arg, (void*)&modem_sig, sizeof(unsigned char));
		if (ret) {
			PRINT(KERN_ERR, "TIOCSERGETMSR failed to copy write info to user "
				"(arg=0x%lx ret=%lu)\n", arg, ret);
			retval = -1;
		}
	}
	break;
	
	case TIOGETREGISTER:
	{
		unsigned int reg;
		int offset;

                ret = copy_from_user((void*)&offset, (void*)arg, sizeof(int));
                if (ret) {
                        PRINT(KERN_ERR, "TIOGETREGISTER failed to copy write info from user "
                                "(arg=0x%lx ret=%lu)\n", arg, ret);
                        retval = -1;
			break;
                } 
		if(!sync_port->flag_escc)
                {
                	reg = sync_port->api.read_reg_char((void *)(sync_port->iusc + offset));
                }
                else
                {
                        reg = sync_port->api.read_reg_char (sync_port->escc.reg_address + offset);
                            
                }
		ret = copy_to_user((void*)arg, (void*)&reg, sizeof(unsigned int));
                if (ret) {
                        PRINT(KERN_ERR, "TIOGETREGISTER failed to copy write info to user "
                                "(arg=0x%lx ret=%lu)\n", arg, ret);
                        retval = -1;
                }
        }
        break;


	case TIOSETREGISTER:
        {
		struct set_reg_struct {
                	unsigned char value;
                	int offset;
		};

		struct set_reg_struct set_reg;

                ret = copy_from_user((void*)&set_reg, (void*)arg, sizeof(struct set_reg_struct));
                if (ret) {
                        PRINT(KERN_ERR, "TIOGETREGISTER failed to copy write info from user "
                                "(arg=0x%lx ret=%lu)\n", arg, ret);
                        retval = -1;
                        break;
                }
                if(!sync_port->flag_escc)
                {
                        sync_port->api.write_reg_char((void *)(sync_port->iusc + set_reg.offset), set_reg.value);
                }
                else
                {
                        sync_port->api.write_reg_char (sync_port->escc.reg_address + set_reg.offset, set_reg.value);

                }
        }
        break;



	case TIOSETCLRRTS:
	{
		int modem_sig = 0;

		ret = copy_from_user((void*)&modem_sig, (void*)arg, sizeof(int));
		if (ret) {
			PRINT(KERN_ERR, "TIOCMSET failed to copy write info from user "
				"(arg=0x%lx ret=%lu)\n", arg, ret);
			retval = -1;
		} else {
			if(modem_sig)
			{
				if(!sync_port->flag_escc)
					iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 1);
				else
					escc_set_rts(&sync_port->escc, &sync_port->api, 1);
			}
			else {
				if(!sync_port->flag_escc)
					iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 0);
				else
					escc_set_rts(&sync_port->escc, &sync_port->api, 0);
			}

		}
		break;
	}

	case TIOSETCLRDTR:
	{
		int modem_sig = 0;

		ret = copy_from_user((void*)&modem_sig, (void*)arg, sizeof(int));
		if (ret) {
			PRINT(KERN_ERR, "TIOCMSET failed to copy write info from user "
				"(arg=0x%lx ret=%lu)\n", arg, ret);
			retval = -1;
		} else {
			if(modem_sig)
			{
				if(!sync_port->flag_escc)
					iusc_set_dtr((void*)sync_port->iusc, &sync_port->api, 1);
				else
					escc_set_dtr(&sync_port->escc, &sync_port->api, 1);
			}
			else {
				if(!sync_port->flag_escc)
					iusc_set_dtr((void*)sync_port->iusc, &sync_port->api, 0);
				else
					escc_set_dtr(&sync_port->escc, &sync_port->api, 0);
			}
		}
		break;
	}

	case TCFLSH:
		switch (arg) {
		case TCIFLUSH:
			/* input buffer */
			#if (LINUX_VERSION_CODE > 132608)
			spin_lock_irqsave(&sync_port->driver_lock, flags);
			#else
			save_flags(flags); cli();
			#endif
			if(!sync_port->flag_escc)
				DO_CCAR_CMD(sync_port->iusc, PURGE_RX_FIFO, &sync_port->api);
			sync_port->rx.head = 0;
			sync_port->rx.tail = 0;
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif
			break;

		case TCIOFLUSH:
			/* input & output */
			#if (LINUX_VERSION_CODE > 132608)
			spin_lock_irqsave(&sync_port->driver_lock, flags);
			#else
			save_flags(flags); cli();
			#endif
			if(!sync_port->flag_escc)
				DO_CCAR_CMD(sync_port->iusc, PURGE_RX_FIFO, &sync_port->api);
			sync_port->rx.head = 0;
			sync_port->rx.tail = 0;
			sync_port->tx.cur_buf_slot = sync_port->tx.buffer;
			sync_port->tx.len = 0;
			if(!sync_port->flag_escc)
				DO_CCAR_CMD(sync_port->iusc, PURGE_TX_FIFO, &sync_port->api);
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif
			break;

		case TCOFLUSH:
			/* output */
			#if (LINUX_VERSION_CODE > 132608)
			spin_lock_irqsave(&sync_port->driver_lock, flags);
			#else
			save_flags(flags); cli();
			#endif
			sync_port->tx.cur_buf_slot = sync_port->tx.buffer;
			sync_port->tx.len = 0;
			if(!sync_port->flag_escc)
				DO_CCAR_CMD(sync_port->iusc, PURGE_TX_FIFO, &sync_port->api);
			#if (LINUX_VERSION_CODE > 132608)
			spin_unlock_irqrestore(&sync_port->driver_lock, flags);
			#else
			restore_flags(flags);
			#endif
			break;

		default:
			retval = -EINVAL;
		}
		break;

	default:
		retval = -ENOSYS; /* Function not implemented. */
		break;
	}
	return retval;
}


// poll function - called when the "file" /dev/csync uses poll/select in userspace
static unsigned int comsync_poll(struct file *file, poll_table *wait)
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,18,0))	
	struct inode *inode = file->f_dentry->d_inode;
	int port = ((NUM(file->f_dentry->d_inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
  int board = (NUM(file->f_dentry->d_inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#else 
	struct inode *inode = file_inode(file);
	int port = ((NUM(inode->i_rdev) + 2) % NUM_COMSYNC_PORT) ? 0 : 1;
	int board = (NUM(inode->i_rdev) - 1)/ NUM_COMSYNC_PORT;
#endif
	struct sync_port_drv_struct *sync_port = &bus_driver[board]->sync_port_driver[port];
	unsigned int mask = 0;


	poll_wait(file, &sync_port->rx.wait,  wait);
	poll_wait(file, &sync_port->tx.wait, wait);
	if (CIRC_BUF_DATA_AVAIL(sync_port->rx.head, sync_port->rx.tail, BUF_SIZE))
		mask |= POLLIN | POLLRDNORM;	/* readable */
	if (!sync_port->tx.len)
		mask |= POLLOUT | POLLWRNORM;	/* writable */

	return mask;
}

// interrupt handler
static int comsync_interrupt_handler(int irq, void *context)
{
	struct bus_driver_struct *bus_driver_context = (struct bus_driver_struct *)context;

	iusc_isr_handler(bus_driver_context);

	return IRQ_HANDLED;
}

// define which file operations are supported
struct file_operations comsync_fops = {
	.owner	=	THIS_MODULE,
	.llseek	=	NULL,
	.read		=	comsync_read,
	.write		=	comsync_write,
//	.readdir	=	NULL,
	.poll		=	comsync_poll,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))		
	.unlocked_ioctl = comsync_ioctl,
#else
	.ioctl		=	comsync_ioctl,
#endif
	.mmap		=	NULL,
	.open		=	comsync_open,
	.flush		=	NULL,
	.release	=	comsync_release,
	.fsync		=	NULL,
	.fasync		=	NULL,
	.lock		=	NULL,
	//.readv		=	NULL,
	//.writev		=	NULL,
};

int init_escc(unsigned int iobase, unsigned int irq)
{
	int ret, i, j;	
    int *api_ret=NULL;
    unsigned char reg;

	if(iobase)
	{		
		api_ret = (int *) request_region(iobase, COMSYNC_ESCC_IO_SIZE, COMSYNC_DRV_NAME);
		if (!api_ret)
        {
            printk ("Allocation for I/O port range is failed: Try other range\n");
            return (-ENOMEM);
        }
		bus_driver[num_comsync_board] = kmalloc( sizeof(struct bus_driver_struct), GFP_KERNEL);
		if(bus_driver[num_comsync_board])
			memset(bus_driver[num_comsync_board], 0, sizeof(struct bus_driver_struct));
		else {
			printk(KERN_ERR DBGPRINT_HEAD "bus_driver  kmalloc failed.");
			iobase = 0;
			return -ENOMEM;
		}		

		bus_driver[num_comsync_board]->api.read_reg_char = CTI_read_io_char;
		bus_driver[num_comsync_board]->api.read_reg_word = CTI_read_io_word;
		bus_driver[num_comsync_board]->api.read_reg_long = CTI_read_io_long;
		bus_driver[num_comsync_board]->api.write_reg_char = CTI_write_io_char;
		bus_driver[num_comsync_board]->api.write_reg_word = CTI_write_io_word;
		bus_driver[num_comsync_board]->api.write_reg_long = CTI_write_io_long;
		bus_driver[num_comsync_board]->escc_base_address = iobase;
		for(j = 0; j < NUM_COMSYNC_PORT; j++)
		{
			bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL|GFP_DMA);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}

			bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL|GFP_DMA);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_char = CTI_read_io_char;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_word = CTI_read_io_word;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_long = CTI_read_io_long;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_char = CTI_write_io_char;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_word = CTI_write_io_word;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_long = CTI_write_io_long;
			bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address = (char *)iobase;
			bus_driver[num_comsync_board]->sync_port_driver[j].escc.reg_address = (char *)(iobase + j*COMSYNC_ESCC_PORT_IO_SIZE + COMSYNC_ESCC_PORT_IO_OFFSET);
			bus_driver[num_comsync_board]->sync_port_driver[j].escc.index = j;
			bus_driver[num_comsync_board]->sync_port_driver[j].escc.use_dma_write = 0;
			bus_driver[num_comsync_board]->sync_port_driver[j].escc.use_dma_read = 0;
			bus_driver[num_comsync_board]->sync_port_driver[j].flag_escc = 1;

			printk(KERN_INFO DBGPRINT_HEAD "%s %s %s%d initialized\n", comsync_name, comsync_version, COMSYNC_DRV_NAME, (num_comsync_board * NUM_COMSYNC_PORT + j + 1));
			bus_driver[num_comsync_board]->sync_port_driver[j].status_device = DEV_READY;
		}
		for(i=0;i<num_comsync_board;i++)
		{
			if(bus_driver[i]->comsync_irq == irq)
				goto skip_irq_request;
		}
		bus_driver[num_comsync_board]->comsync_irq = irq;
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))
		ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)escc_interrupt_handler,
			IRQF_SHARED, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
		#else
		ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)escc_interrupt_handler,
			SA_INTERRUPT|SA_SHIRQ, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
		#endif
		if(ret) {
			PRINT(KERN_ERR,
				"[%u] request_irq failed irq %u (%d)\n",
				num_comsync_board, irq, ret);
			bus_driver[num_comsync_board]->comsync_irq = 0;
			num_comsync_board++;
			return ret;
		}
skip_irq_request:
		reg  = 0;
		switch(irq)
		{
		case 3:
			reg |=0x00;
			break;
		case 5:
			reg |=0x08;
			break;
		case 7:
			reg |=0x10;
			break;
		case 9:
			reg |=0x18;
			break;
		case 10:
			reg |=0x20;
			break;
		case 11:
			reg |=0x28;
			break;
		case 12:
			reg |=0x30;
			break;
		case 15:
			reg |=0x38;
			break;
		}

		reg |=0xc3; //enable board's DMA capability
		bus_driver[num_comsync_board]->api.write_reg_char(bus_driver[num_comsync_board]->escc_base_address + CS104_CtrlStat, reg);
		// enable board's master interrupt, not same as register setting of MIE
		bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc, 0x00);	// need to reset first
		reg = bus_driver[num_comsync_board]->api.read_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc);
		reg |= 0x40;	//MIE
		reg |= 0x80; // TC int for DMA
		bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc, reg);
		//reg = bus_driver[num_comsync_board]->api.read_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_IRQ);
		//bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_IRQ, reg | 0x01);//shared IRQ
		//Reset Hardware
		bus_driver[num_comsync_board]->api.write_reg_char(bus_driver[num_comsync_board]->escc_base_address + 0x29,0x30);
		bus_driver[num_comsync_board]->api.write_reg_char(bus_driver[num_comsync_board]->escc_base_address+0x2c,0x55);
		reg = bus_driver[num_comsync_board]->api.read_reg_char(bus_driver[num_comsync_board]->escc_base_address+0x2c);		
		num_comsync_board++;
		if (reg != 0x55)
		{
			printk("Comsync-104 iobase 0x%x is incorrect, check jumper settings\n", iobase);
			return -ENOMEM;
		}
	}
	return 0;
}
#define COMSYNCIII_VID 0x11AA
#define COMSYNCIII_DID 0x1770
#define COMSYNCIII_SUBV_ID 0x0820
int init_comsyncIII_escc(void)
{
   int ret, j, k;	
   unsigned char reg = 0;
   struct pci_dev *dev = NULL;
   unsigned long regl;
		
   dev = pci_get_subsys(COMSYNCIII_VID, COMSYNCIII_DID, CTI_SUB_VENDOR_ID, PCI_ANY_ID, dev);	
   if (!dev) return 0;
   printk("Found Comsync-III device\n");
   bus_driver[num_comsync_board] = kmalloc( sizeof(struct bus_driver_struct), GFP_KERNEL);
	if(bus_driver[num_comsync_board])
		memset(bus_driver[num_comsync_board], 0, sizeof(struct bus_driver_struct));
	else {
		printk(KERN_ERR DBGPRINT_HEAD "bus_driver  kmalloc failed.");			
		return -ENOMEM;
	}	
   bus_driver[num_comsync_board]->pci_dev_struct = dev;
   //pci_disable_device(dev);
   
    bus_driver[num_comsync_board]->bar2_start = pci_resource_start(dev, 2);
    bus_driver[num_comsync_board]->bar2_len = pci_resource_end(dev, 2) - bus_driver[num_comsync_board]->bar2_start;
	//printk("bar2 start %lx len %lx\n",bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len);
	if(!request_mem_region(bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len, COMSYNC_DRV_NAME)){
	   printk ("Allocation for I/O memory range is failed: Try other range\n");
	   return -ENOMEM;
	}
	bus_driver[num_comsync_board]->comsyncIII_bar2 = ioremap(bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len);
	

	bus_driver[num_comsync_board]->bar1_start = pci_resource_start(dev, 1);
	bus_driver[num_comsync_board]->bar1_len = pci_resource_end(dev, 1) - bus_driver[num_comsync_board]->bar1_start;
	//printk("bar1 start %lx len %lx\n",bus_driver[num_comsync_board]->bar1_start, bus_driver[num_comsync_board]->bar1_len);
    if(!request_mem_region(bus_driver[num_comsync_board]->bar1_start, bus_driver[num_comsync_board]->bar1_len, COMSYNC_DRV_NAME)){
	   printk ("Allocation for I/O memory range is failed: Try other range\n");
	   return -ENOMEM;
	}
	bus_driver[num_comsync_board]->comsyncIII_bar1 = ioremap(bus_driver[num_comsync_board]->bar1_start, bus_driver[num_comsync_board]->bar1_len);
	
    bus_driver[num_comsync_board]->bar0_start = pci_resource_start(dev, 0);
	bus_driver[num_comsync_board]->bar0_len = pci_resource_end(dev, 0) - bus_driver[num_comsync_board]->bar0_start;
	//printk("bar0 start %lx len %lx\n",bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len);
	if(!request_mem_region(bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len, COMSYNC_DRV_NAME)){
	   printk ("Allocation for I/O memory range is failed: Try other range\n");
	   return -ENOMEM;
	}
    bus_driver[num_comsync_board]->escc_base_address = ioremap(bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len);
    
	ret = pci_enable_device(dev);
	if(ret)
	{
	   printk ("pci_enable_device failed ret %d\n",ret);
	   return -ENOMEM;
	}		

	bus_driver[num_comsync_board]->api.read_reg_char = CTI_read_char;
	bus_driver[num_comsync_board]->api.read_reg_word = CTI_read_word;
	bus_driver[num_comsync_board]->api.read_reg_long = CTI_read_long;
	bus_driver[num_comsync_board]->api.write_reg_char = CTI_write_char;
	bus_driver[num_comsync_board]->api.write_reg_word = CTI_write_word;
	bus_driver[num_comsync_board]->api.write_reg_long = CTI_write_long;

	
	for(j = 0; j < NUM_COMSYNC_PORT; j++)
	{
		bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL|GFP_DMA);
		if(!bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer) {
			printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
			return -ENOMEM;
		}
		bus_driver[num_comsync_board]->sync_port_driver[j].rx.sdlc_frames.info = kmalloc( BUF_SIZE * sizeof( struct _sdlc_frame_info), GFP_KERNEL);
		if(!bus_driver[num_comsync_board]->sync_port_driver[j].rx.sdlc_frames.info) {
			printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
			return -ENOMEM;
		}
		bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL|GFP_DMA);
		if(!bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer) {
			printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
			return -ENOMEM;
		}
		for(k = 0; k < NUM_DMA_LIST+1; k++)
		{
			bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_buffer[k] = kmalloc( DMA_BUF_REQEST, GFP_KERNEL|GFP_DMA);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_buffer[k]) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}
			bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_phy_addr[k] = virt_to_phys(bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_buffer[k]);
		
			bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_buffer[k] = kmalloc( DMA_BUF_REQEST, GFP_KERNEL|GFP_DMA);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_buffer[k]) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}
			bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_phy_addr[k] = virt_to_phys(bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_buffer[k]);			
		}
		bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_char = CTI_read_char;
		bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_word = CTI_read_word;
		bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_long = CTI_read_long;
		bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_char = CTI_write_char;
		bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_word = CTI_write_word;
		bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_long = CTI_write_long;
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address = bus_driver[num_comsync_board]->escc_base_address;
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.reg_address = (bus_driver[num_comsync_board]->escc_base_address + j*COMSYNC_ESCC_PORT_IO_SIZE + COMSYNC_ESCC_PORT_IO_OFFSET);
		bus_driver[num_comsync_board]->sync_port_driver[j].tx.fifo = bus_driver[num_comsync_board]->comsyncIII_bar1 + j * 0x400;
		bus_driver[num_comsync_board]->sync_port_driver[j].rx.fifo = bus_driver[num_comsync_board]->comsyncIII_bar1 + j * 0x400 + 0x200;
		for(k = 0; k < NUM_DMA_LIST; k++)
		{		  	
			bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_count_addr[k] = bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address + 0x50 + j*0x20 + k*8;
	  		bus_driver[num_comsync_board]->sync_port_driver[j].rx.dma_list_addr[k] = bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address + 0x54 + j*0x20 + k*8;	 
	  		bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_count_addr[k] = bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address + 0x40 + j*0x20 + k*8;
	  		bus_driver[num_comsync_board]->sync_port_driver[j].tx.dma_list_addr[k] = bus_driver[num_comsync_board]->sync_port_driver[j].escc.base_address + 0x44 + j*0x20 + k*8;	  		 		
		}
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.index = j;
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.use_dma_write = 0;
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.use_dma_read = 0;
		bus_driver[num_comsync_board]->sync_port_driver[j].flag_escc = 1;
		bus_driver[num_comsync_board]->sync_port_driver[j].escc.flag_comsync_III = 1;

		printk(KERN_INFO DBGPRINT_HEAD "%s %s %s%d initialized\n", comsync_name, comsync_version, COMSYNC_DRV_NAME, (num_comsync_board * NUM_COMSYNC_PORT + j + 1));
		bus_driver[num_comsync_board]->sync_port_driver[j].status_device = DEV_READY;
	}
	
	bus_driver[num_comsync_board]->comsync_irq = dev->irq;
	printk("comsync: IRQ %d\n",bus_driver[num_comsync_board]->comsync_irq);
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))
	ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)escc_interrupt_handler,
		 IRQF_SHARED, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
	#else
	ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)escc_interrupt_handler,
		SA_INTERRUPT|SA_SHIRQ, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
	#endif
	if(ret) {
		PRINT(KERN_ERR,
			"[%u] request_irq failed irq %u (%d)\n",
			num_comsync_board, bus_driver[num_comsync_board]->comsync_irq, ret);
		return ret;
	}				

	//Reset Hardware
	bus_driver[num_comsync_board]->api.write_reg_char(bus_driver[num_comsync_board]->escc_base_address + 0x29,0x30);		
	// enable board's master interrupt, not same as register setting of MIE
	bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc, 0x00);	// need to reset first
	reg = bus_driver[num_comsync_board]->api.read_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc);
	reg |= 0x40;	//MIE	
	// enable MIE
	bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + CS104_Misc, reg);	
	//enable fifo interrupt
	bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + 0xa8, 0x11); 
	// enable actel master interrupt
	bus_driver[num_comsync_board]->api.write_reg_char (bus_driver[num_comsync_board]->escc_base_address + 0x9c, DMA_INTR_ENAB|FIFO_INTR_ENAB|ESCC_INTR_ENAB);	
	
	//enable PCI bus master
	reg = bus_driver[num_comsync_board]->api.read_reg_char(bus_driver[num_comsync_board]->comsyncIII_bar2 + 0x04);
	pci_write_config_byte(dev,0x04, (reg | 0x04));
		
	//enable PCI core interrupt
	regl = bus_driver[num_comsync_board]->api.read_reg_long(bus_driver[num_comsync_board]->comsyncIII_bar2 + 0x5c);
	bus_driver[num_comsync_board]->api.write_reg_long(bus_driver[num_comsync_board]->comsyncIII_bar2+0x5c, 0x8000|regl);
		
	num_comsync_board++;
			
	return 0;
}

// initialize module (and interrupt)
static int __init comsync_init_module (void) {
	int i, j;
	int ret;
	struct pci_dev *dev = NULL;
   unsigned long plx_reg;
   int *api_ret=NULL;
   
   	
	for (num_comsync_board=0; num_comsync_board < MAX_COMSYNC_BOARD;)
	{
		dev = pci_get_subsys(VENDOR_ID,DEVICE_ID, CTI_SUB_VENDOR_ID, PCI_ANY_ID, dev);		
    if (!dev) break;
      
    printk("found comsync device\n");


    bus_driver[num_comsync_board] = kmalloc( sizeof(struct bus_driver_struct), GFP_KERNEL);
		if(bus_driver[num_comsync_board])
			memset(bus_driver[num_comsync_board], 0, sizeof(struct bus_driver_struct));
		else {
			printk(KERN_ERR DBGPRINT_HEAD "bus_driver  kmalloc failed.");
			return -ENOMEM;
		}
		bus_driver[num_comsync_board]->pci_dev_struct = dev;
		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
		pci_disable_device(dev);
		#endif
		bus_driver[num_comsync_board]->bar2_start = pci_resource_start(dev, 2);
		bus_driver[num_comsync_board]->bar2_len = pci_resource_end(dev, 2) - bus_driver[num_comsync_board]->bar2_start;
		//printk("bar2 len %lx\n",bus_driver[num_comsync_board]->bar2_len);
		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
		api_ret = (int *)request_mem_region(bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len, COMSYNC_DRV_NAME);
		if (!api_ret)
    {
         printk ("Allocation for BAR2 I/O memory range is failed: Try other range\n");
         return (ret);
    }
		#endif

		bus_driver[num_comsync_board]->bar0_start = pci_resource_start(dev, 0);
		bus_driver[num_comsync_board]->bar0_len = pci_resource_end(dev, 0) - bus_driver[num_comsync_board]->bar0_start;
		//printk("bar0 start %lx len %lx\n",bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len);
		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
		api_ret = (int *) request_mem_region(bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len, COMSYNC_DRV_NAME);
		if (!api_ret)
    {
         printk ("Allocation for BAR0 I/O memory range is failed: Try other range\n");
         return (ret);
    }
		#endif
		ret = pci_enable_device(dev);
		if(ret)
		{
			 printk ("pci_enable_device failed ret %d\n",ret);
			 return (ret);
		}

		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
		bus_driver[num_comsync_board]->PLXAperture = (_PLX_registers  *)ioremap(bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len);
		bus_driver[num_comsync_board]->b_iusc = (void*)ioremap(bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len);
		#else
		bus_driver[num_comsync_board]->PLXAperture = (_PLX_registers  *)ioremap_nocache(bus_driver[num_comsync_board]->bar0_start, bus_driver[num_comsync_board]->bar0_len);
		bus_driver[num_comsync_board]->b_iusc = (void*)ioremap_nocache(bus_driver[num_comsync_board]->bar2_start, bus_driver[num_comsync_board]->bar2_len);
		#endif
		bus_driver[num_comsync_board]->b_pld = (bus_driver[num_comsync_board]->b_iusc + PLD_OFFSET);
		bus_driver[num_comsync_board]->api.read_reg_char = CTI_read_char;
		bus_driver[num_comsync_board]->api.read_reg_word = CTI_read_word;
		bus_driver[num_comsync_board]->api.read_reg_long = CTI_read_long;
		bus_driver[num_comsync_board]->api.write_reg_char = CTI_write_char;
		bus_driver[num_comsync_board]->api.write_reg_word = CTI_write_word;
		bus_driver[num_comsync_board]->api.write_reg_long = CTI_write_long;

		for(j = 0; j < NUM_COMSYNC_PORT; j++)
		{
			bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].rx.buffer) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}			
			bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer = kmalloc( BUF_SIZE, GFP_KERNEL);
			if(!bus_driver[num_comsync_board]->sync_port_driver[j].tx.buffer) {
				printk(KERN_ERR DBGPRINT_HEAD "sync_port_driver  kmalloc failed.");
				return -ENOMEM;
			}
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_char = CTI_read_char;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_word = CTI_read_word;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.read_reg_long = CTI_read_long;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_char = CTI_write_char;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_word = CTI_write_word;
			bus_driver[num_comsync_board]->sync_port_driver[j].api.write_reg_long = CTI_write_long;
			bus_driver[num_comsync_board]->sync_port_driver[j].iusc = bus_driver[num_comsync_board]->b_iusc + IUSC_OFFSET(j);

			bus_driver[num_comsync_board]->sync_port_driver[j].pld = bus_driver[num_comsync_board]->b_pld;
			printk(KERN_INFO DBGPRINT_HEAD "%s %s %s%d initialized\n", comsync_name, comsync_version, COMSYNC_DRV_NAME, (num_comsync_board * NUM_COMSYNC_PORT + j + 1));
			iusc_reset(j, (void*)bus_driver[num_comsync_board]->sync_port_driver[j].iusc, (void*)bus_driver[num_comsync_board]->b_pld, &bus_driver[num_comsync_board]->api);
			bus_driver[num_comsync_board]->sync_port_driver[j].status_device = DEV_READY;
		}
		// IRQ hook
		bus_driver[num_comsync_board]->comsync_irq = dev->irq;
		printk("comsync: IRQ %d\n",bus_driver[num_comsync_board]->comsync_irq);
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))
		ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)comsync_interrupt_handler,
			 IRQF_SHARED, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
		#else
		ret = request_irq(bus_driver[num_comsync_board]->comsync_irq, (void*)comsync_interrupt_handler,
			SA_INTERRUPT|SA_SHIRQ, COMSYNC_DRV_NAME, bus_driver[num_comsync_board]);
		#endif
		if(ret) {
			PRINT(KERN_ERR,
				"[%u] request_irq failed irq %u (%d)\n",
				num_comsync_board, bus_driver[num_comsync_board]->comsync_irq, ret);
			return ret;
		}
		num_comsync_board++;
	}

	for (i=0; i<num_comsync_board; i++)
	{
		set_pld_int(1, (void*)bus_driver[i]->b_pld, &bus_driver[i]->api);
		//	Map the PLX Local Interrupt pin (LINT#) to be passed through to the PCI Bus INTA# pin
		plx_reg = (bus_driver[i]->api).read_reg_long(&(bus_driver[i]->PLXAperture->INTCSR));
		//printk("plx_reg %x\n",plx_reg);
		plx_reg &= ~(1 << 16);				//	disable the LINT# pin as an output
		plx_reg |= (1 << 11) | (1 << 8);	//	enable the LINT# pin and the INTA# output
		//(bus_driver[i]->api).write_reg_long(&(bus_driver[i]->PLXAperture->INTCSR), plx_reg);
        //printk("plx_reg %x\n",plx_reg);
		plx_reg = (bus_driver[i]->api).read_reg_long(&(bus_driver[i]->PLXAperture->INTCSR));
        //printk("plx_reg %x\n",plx_reg);
	}

	ret = init_comsyncIII_escc();
   	if(ret)
   		return ret;
   	
	if(!num_comsync_board)
	{
		printk("No Comsync/PCI device found!\n");
	}


	if(iobase1 && irq1)
		init_escc(iobase1, irq1);
	if(iobase2 && irq2)
		init_escc(iobase2, irq2);
	if(iobase3 && irq3)
		init_escc(iobase3, irq3);
	if(iobase4 && irq4)
		init_escc(iobase4, irq4);

	if(!num_comsync_board)
	{
		printk("No Comsync/104 device found!\n");
		return 0;
	}
	else
	{
		init_timer(&sync_timer);
		sync_timer.function = rx_char_scoop_timer;
	}

	i = register_chrdev (comsync_major, COMSYNC_DRV_NAME, &comsync_fops);
	if (i != 0) return - EIO;

	
	printk("%s %s initialized.\n", comsync_name, comsync_version);

	return 0;
}

// close and cleanup module
static void __exit comsync_cleanup_module (void) {
	int i, j, k;

	printk("Cleaning up Comsync module...\n");

	if(num_comsync_board)
	{
		del_timer_sync(&sync_timer);	

		for (i=0; i<num_comsync_board; i++)
		{					
			if(bus_driver[i]->escc_base_address)
			{
				bus_driver[i]->api.write_reg_char (bus_driver[i]->escc_base_address + CS104_Misc, 0x00);
				bus_driver[i]->api.write_reg_char(bus_driver[i]->escc_base_address + 0x29,0x30);
				
				if(bus_driver[i]->sync_port_driver[0].escc.flag_comsync_III)
				{
					pci_disable_device(bus_driver[i]->pci_dev_struct);
					#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
					release_mem_region (bus_driver[i]->bar0_start, bus_driver[i]->bar0_len);	
					release_mem_region (bus_driver[i]->bar1_start, bus_driver[i]->bar1_len);	
					release_mem_region (bus_driver[i]->bar2_start, bus_driver[i]->bar2_len);				
					#endif
				}
				else
					release_region ((unsigned int)bus_driver[i]->escc_base_address, COMSYNC_ESCC_IO_SIZE);
			}
			else
			{
				for(j = 0; j < NUM_COMSYNC_PORT; j++)
					iusc_reset(j, bus_driver[i]->b_iusc, bus_driver[i]->b_pld, &bus_driver[i]->api);
				pci_disable_device(bus_driver[i]->pci_dev_struct);
				#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0))
				release_mem_region (bus_driver[i]->bar2_start, bus_driver[i]->bar2_len);
				release_mem_region (bus_driver[i]->bar0_start, bus_driver[i]->bar0_len);
				#endif
			}
			
			if(bus_driver[i]->comsync_irq)
				free_irq(bus_driver[i]->comsync_irq, bus_driver[i]);

			for(j = 0; j < NUM_COMSYNC_PORT; j++)
			{				
				if(bus_driver[i]->sync_port_driver[j].rx.buffer)
					kfree(bus_driver[i]->sync_port_driver[j].rx.buffer);
				if(bus_driver[i]->sync_port_driver[j].rx.sdlc_frames.info)
					kfree(bus_driver[i]->sync_port_driver[j].rx.sdlc_frames.info);
					
				for(k = 0; k < NUM_DMA_LIST+1; k++)
				{
					if(bus_driver[i]->sync_port_driver[j].rx.dma_buffer[k])
						kfree(bus_driver[i]->sync_port_driver[j].rx.dma_buffer[k]);
					if(bus_driver[i]->sync_port_driver[j].tx.dma_buffer[k])
						kfree(bus_driver[i]->sync_port_driver[j].tx.dma_buffer[k]);
				}
				if(bus_driver[i]->sync_port_driver[j].tx.buffer)
					kfree(bus_driver[i]->sync_port_driver[j].tx.buffer);
			}
			kfree(bus_driver[i]);			
		}
	}	

	unregister_chrdev (comsync_major, COMSYNC_DRV_NAME);
}

module_init(comsync_init_module);
module_exit(comsync_cleanup_module);

module_param(iobase1, uint, 0644);
MODULE_PARM_DESC(iobase1, "I/O base address for Comsync/104 board 1");
module_param(irq1, uint, 0644);
MODULE_PARM_DESC(irq1, "irq number for Comsync/104 board 1");
module_param(iobase2, uint, 0644);
MODULE_PARM_DESC(iobase2, "I/O base address for Comsync/104 board 2");
module_param(irq2, uint, 0644);
MODULE_PARM_DESC(irq2, "irq number for Comsync/104 board 2");
module_param(iobase3, uint, 0644);
MODULE_PARM_DESC(iobase3, "I/O base address for Comsync/104 board 3");
module_param(irq3, uint, 0644);
MODULE_PARM_DESC(irq3, "irq number for Comsync/104 board 3");
module_param(iobase4, uint, 0644);
MODULE_PARM_DESC(iobase4, "I/O base address for Comsync/104 board 4");
module_param(irq4, uint, 0644);
MODULE_PARM_DESC(irq4, "irq number for Comsync/104 board 4");

