/************************************************************
**	File:	isr-escc.c
**	Author:
**			Hasan Jamal
**
**	Description:
**		This module contains the code that is very specific to ISR
**		operations of ESCC.
**
**	Revision history:
**		Original:	11/24/10
**
**	THIS IS THE UNPUBLISHED PROPRIETARY SOURCE CODE OF CONNECT TECH INC.
**	The copyright notice above does not evidence any actual or intended
**	publication of such source code.
**
**	This module contains Proprietary Information of Connect Tech, Inc
**	and should be treated as Confidential.
************************************************************/

#include "precomp.h"

#include <linux/sched.h>
#include <linux/irqreturn.h>

int escc_dma_prepare(int channel, int mode, void *buf, unsigned int count)
/*++

Routine Description:

    This routine is used to prepare the channel for a dma request

Arguments:

	chaneel - dma channel number.
	mode - DMA_MODE_WRITE or DMA_MODE_READ
	buf - data buffer
	count - number bytes for the request

Return Value:

    None.

--*/
{
	unsigned long flags;

	flags = claim_dma_lock( );
	disable_dma(channel);
	clear_dma_ff(channel);
	set_dma_mode(channel, mode);
	set_dma_addr(channel, virt_to_bus(buf));
	set_dma_count(channel, count);
	enable_dma(channel);
	release_dma_lock(flags);
	return 0;
}

int escc_dma_cancel(int channel)
/*++

Routine Description:

    This routine is used to cancel a dma request

Arguments:

	chaneel - dma channel number.


Return Value:

    None.

--*/
{
	unsigned long flags;

	flags = claim_dma_lock( );
	disable_dma(channel);
	clear_dma_ff(channel);
	release_dma_lock(flags);
	return 0;
}

int escc_dma_isdone(int channel)
/*++

Routine Description:

    This routine is used to check the status of a dma request

Arguments:

	chaneel - dma channel number.


Return Value:

    None.

--*/
{
	int residue;
	unsigned long flags = claim_dma_lock ( );
	residue = get_dma_residue(channel);
	release_dma_lock(flags);
	return (residue );
}

void
start_DMA_write(
    struct sync_port_drv_struct *sync_port
    )

/*++

Routine Description:

    This will trigger DMA mechanism to write data


Arguments:

    sync_port - a pointer to the sync_port_drv_struct.

Return Value:

    None.

--*/

{
	struct _api_context *api = &sync_port->api;

	//printk("start_DMA_write\n");

	sync_port->escc.write_dma_busy = 1;

	escc_dma_prepare(sync_port->escc.write_dma_ch, DMA_MODE_WRITE, (void *)sync_port->tx.buffer, sync_port->tx.len);

	//enable transmitter
	if (!sync_port->escc.WriteReg.r5.wr5.bf.tx_enab)
	{
		sync_port->escc.WriteReg.r5.wr5.bf.tx_enab = 1;
		api->write_reg_char (sync_port->escc.reg_address+REG5, sync_port->escc.WriteReg.r5.wr5.reg);
	}

	if (sync_port->port_settings.smode.tx  == SMODE_MONO && sync_port->port_settings.crc.tx_type)
	{
		api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_UR_EOM);
	}

	if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE)
	{
		sync_port->escc.WriteReg.r5.wr5.bf.rts = 1;
		api->write_reg_char(sync_port->escc.reg_address+REG5,sync_port->escc.WriteReg.r5.wr5.reg);
	}

	start_dma_dtr_req(&sync_port->escc, api);
}

void
start_DMA_read(
    struct sync_port_drv_struct *sync_port
    )

/*++

Routine Description:

    This will trigger DMA mechanism to read data


Arguments:

    sync_port - pointer to the sync_port_drv_struct.

Return Value:

    None.

--*/

{
	struct _api_context *api = &sync_port->api;

	sync_port->escc.dma_read_size = BUF_SIZE - sync_port->rx.head;
	if(sync_port->escc.dma_read_size > DMA_READ_SIZE)
		sync_port->escc.dma_read_size = DMA_READ_SIZE;

	sync_port->escc.read_dma_busy = 1;

	escc_dma_prepare(sync_port->escc.read_dma_ch, DMA_MODE_READ, (void *)(sync_port->rx.buffer + sync_port->rx.head), sync_port->escc.dma_read_size);

	if (!sync_port->escc.WriteReg.r3.wr3.bf.rx_enab)
	{
		sync_port->escc.WriteReg.r3.wr3.bf.rx_enab = 1;
		api->write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
	}

	if (sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode != RX_INT_ON_FIRST_CH_OR_SPEC_CON)
	{
		sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode = RX_INT_ON_SPEC_CON_ONLY;
		api->write_reg_char (sync_port->escc.reg_address+REG1, sync_port->escc.WriteReg.r1.wr1.reg);
	}

	if(sync_port->port_settings.smode.rx != SMODE_ASYNC && !sync_port->escc.hunt_mode)
	{
		sync_port->escc.hunt_mode = 1;
		sync_port->escc.WriteReg.r3.wr3.bf.enter_hunt = 1;
		api->write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
	}

	start_dma_wreq_read(&sync_port->escc, api);
}
void
tx_escc(
	 struct sync_port_drv_struct *sync_port,
	 unsigned char *data,
	 unsigned long len
	)

/*++

Routine Description:

    This routine is used to send data by the driver.

Arguments:

	sync_port - a pointer to the sync_port_drv_struct.
	data - pointer to data buffer
    len - len of write this routine will make, the length will never exceed Tx FIFO (4 bytes) of ESCC.

Return Value:

    None.

--*/
{
	struct _api_context *api = &sync_port->api;

	while(len--)
	{
		// make sure TBE is set when writing to FIFO
		while(!(api->read_reg_char (sync_port->escc.reg_address+REG0) & 0x04));
		api->write_reg_char (sync_port->escc.reg_address+REG8, *data);
		data++;
	}
}



void escc_tx_fifo(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

    This routine is used to start data transmission by the driver from write entry point or non-interrupt context.

Arguments:

	sync_port - a pointer to the sync_port_drv_struct.


Return Value:

    None.

--*/
{
	struct _api_context *api = &sync_port->api;
	unsigned short tx_space;
	unsigned long flags;


	#if (LINUX_VERSION_CODE > 132608)
	spin_lock_irqsave(&sync_port->driver_lock, flags);
	#else
	save_flags(flags); cli();
	#endif
	if (!sync_port->escc.WriteReg.r5.wr5.bf.tx_enab)
	{
		sync_port->escc.WriteReg.r5.wr5.bf.tx_enab = 1;
		api->write_reg_char (sync_port->escc.reg_address+REG5, sync_port->escc.WriteReg.r5.wr5.reg);
	}

	if (sync_port->port_settings.smode.tx  == SMODE_MONO && sync_port->port_settings.crc.tx_type)
		api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_UR_EOM);
	
	tx_space = ESCC_TX_FIFO;
	tx_resume(sync_port, &tx_space);

	#if (LINUX_VERSION_CODE > 132608)
	spin_unlock_irqrestore(&sync_port->driver_lock, flags);
	#else
	restore_flags(flags);
	#endif
}

void
dma_tx_dpc(
    struct sync_port_drv_struct *sync_port
    )

/*++

Routine Description:

    This routine will be called from ISR when TC count interrupt happens. Since there is
	 no way to know for which DMA & channel TC count interrupt happens then DMA progress will
	 be checked to see whether the DMA for transmit for this channel is done.


Arguments:

	sync_port - a pointer to the sync_port_drv_struct.

Return Value:

    None.

--*/
{


	int count;
	struct _api_context *api = &sync_port->api;

	count = escc_dma_isdone(sync_port->escc.write_dma_ch);
	if (count )
		return;


	stop_dma_dtr_req(&sync_port->escc, api);

	if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE)
	{
		sync_port->escc.WriteReg.r5.wr5.bf.rts = 0;
		api->write_reg_char(sync_port->escc.reg_address+REG5,sync_port->escc.WriteReg.r5.wr5.reg);
	}

	sync_port->tx.wakeup = 1;
	sync_port->escc.write_dma_busy = 0;
	sync_port->tx.len = 0;
	sync_port->tx.cur_buf_slot = sync_port->tx.buffer;
	wake_up_interruptible(&sync_port->tx.wait);

}

void
dma_rx_dpc(
    struct sync_port_drv_struct *sync_port, char frame_end
    )

/*++

Routine Description:

    This routine will be called from ISR when TC count interrupt happens. Since there is
	 no way to know for which DMA & channel TC count interrupt happens then DMA progress will
	 be checked to see whether the DMA for receive for this channel is done.


Arguments:

		sync_port - a pointer to the sync_port_drv_struct.

Return Value:

    None.

--*/

{
	unsigned long count;
	struct _api_context *api = &sync_port->api;

	// count will be 0 if DMA is done
	count = escc_dma_isdone(sync_port->escc.read_dma_ch);
	if (count && !frame_end)
	{
		return;
	}

	//printk("Rx DMA complete\n");

	if (!count) {		
		sync_port->rx.head += sync_port->escc.dma_read_size;
		if(sync_port->rx.head >= (BUF_SIZE ))
			sync_port->rx.head = 0;
	}
	else
	{
		escc_dma_cancel(sync_port->escc.read_dma_ch);		
		sync_port->rx.head += sync_port->escc.dma_read_size - count;
		if(sync_port->rx.head >= BUF_SIZE )
			sync_port->rx.head = 0;

	}

	if(frame_end) {
		sync_port->escc.hunt_mode = 1;
		sync_port->escc.WriteReg.r3.wr3.bf.enter_hunt = 1;
		api->write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
	}
	sync_port->escc.read_dma_busy = 0;

	sync_port->rx.wakeup = 1;
	wake_up_interruptible(&sync_port->rx.wait);

	start_DMA_read(sync_port);
}

void handle_tx_int(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine handles transmit interrupt.

Arguments:

	sync_port - points to port driver structure.

Return Value:

	None.

--*/

{	
	unsigned short tx_space;	
	struct _api_context *api = &sync_port->api;
	
	sync_port->tx.tx_int_wait = 0;
	
	if (sync_port->tx.len == 0)
	{
		// reset TX INT
		api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_INT_PEND);
		return;
	}
	
	if (sync_port->escc.flag_comsync_III) 
	{		
		api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_INT_PEND);
		return;
	}

	if (!sync_port->escc.use_dma_write)
	{
		if (sync_port->tx.flow.sholding)
			api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_INT_PEND);
		else {
			tx_space = ESCC_TX_FIFO;
			tx_resume(sync_port, &tx_space);
		}
	}
	else
	{
		// reset TX INT
		if (!sync_port->escc.WriteReg.r5.wr5.bf.tx_enab)
		{
			sync_port->escc.WriteReg.r5.wr5.bf.tx_enab = 1;
			api->write_reg_char (sync_port->escc.reg_address+REG5, sync_port->escc.WriteReg.r5.wr5.reg);
		}
		api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_TX_INT_PEND);
	}
}

void handle_rx_int(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine handles receive interrupt.

Arguments:

	sync_port - points to port driver structure.

Return Value:

	None.

--*/
{	
	struct _api_context *api = &sync_port->api;
	

	if (sync_port->escc.use_dma_read)
	{
		// start DMA
		if (!sync_port->escc.read_dma_busy)
		{
			if (sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode != RX_INT_ON_FIRST_CH_OR_SPEC_CON)
			{
				sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode = RX_INT_ON_SPEC_CON_ONLY;
				api->write_reg_char (sync_port->escc.reg_address+REG1, sync_port->escc.WriteReg.r1.wr1.reg);
			}
			start_DMA_read(sync_port);
		}
		else
		{
			if (sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode != RX_INT_ON_FIRST_CH_OR_SPEC_CON)
			{
				sync_port->escc.WriteReg.r1.wr1.bf.rx_int_mode = RX_INT_ON_SPEC_CON_ONLY;
				api->write_reg_char (sync_port->escc.reg_address+REG1, sync_port->escc.WriteReg.r1.wr1.reg);
			}

			if(sync_port->port_settings.smode.rx != SMODE_ASYNC)
			{
				sync_port->escc.hunt_mode = 1;
				sync_port->escc.WriteReg.r3.wr3.bf.enter_hunt = 1;
				api->write_reg_char (sync_port->escc.reg_address+REG3, sync_port->escc.WriteReg.r3.wr3.reg);
			}
			else
			{
				//invalid condition, DMA is broken
				rx_resume(sync_port, NULL);
			}

		}
		
		return;
	}
	
	sync_port->escc.hunt_mode = 0;

	rx_resume(sync_port, NULL);
}

void handle_ext_int(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine handles external status interrupt.

Arguments:

	sync_port - points to port driver structure.

Return Value:

	None.

--*/
{	
	unsigned char ext_stat, RR0;
	unsigned short tx_space;	
	struct _api_context *api = &sync_port->api;
	
	RR0 = api->read_reg_char (sync_port->escc.reg_address+REG0);	// read ext status, rr0
	//printk("EXT-Stat service, RR0 %x\n",RR0);
	ext_stat = sync_port->escc.ReadReg.r0.rr0.reg ^ RR0;

	if ((ext_stat & SYNC_HUNT) && !(sync_port->escc.flag_comsync_III))
	{
		sync_port->escc.hunt_mode = 0;
		if (RR0 & SYNC_HUNT) {
			if(sync_port->escc.use_dma_read) {
				if(!sync_port->escc.read_dma_busy)
					start_DMA_read(sync_port);
			}
		}
	}

	if (ext_stat & CTS)
	{
		if (RR0 & CTS)
		{
			sync_port->events &= ~EVENT_CTS_FALL;
			sync_port->events |= EVENT_CTS_RISE;
		}
		else
		{
			sync_port->events &= ~EVENT_CTS_RISE;
			sync_port->events |= EVENT_CTS_FALL;
		}
	}

	if (ext_stat & TX_UNDERRUN)
	{		
		sync_port->events |= EVENT_TX_UNDER;
		if (sync_port->tx.len && !sync_port->escc.flag_comsync_III) 
		{									
			if (sync_port->escc.use_dma_write)
			{
				dma_tx_dpc(sync_port);
			}
			else
			{
				tx_space = ESCC_TX_FIFO;
				tx_resume(sync_port, &tx_space);
			}			
		}
	}

	if (ext_stat & BREAK_ABORT)
	{
		if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
			sync_port->events |= EVENT_BREAK;
		} else {
			sync_port->events |= EVENT_ABORT;
		}
	}

	if (ext_stat & DCD)
	{
		if (RR0 & DCD_IS) {
			sync_port->events &= ~EVENT_DCD_FALL;
			sync_port->events |= EVENT_DCD_RISE;
		} else {
			sync_port->events &= ~EVENT_DCD_RISE;
			sync_port->events |= EVENT_DCD_FALL;
		}
	}

	sync_port->escc.ReadReg.r0.rr0.reg = RR0 & 0xfd; // zeroing ZC bit

	/**
	**	User's manual says to do this twice (page 2-33)
	**/
	api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_EXT_STATUS_INT);		// reset ext-status interrupts
	api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_EXT_STATUS_INT);		// reset ext-status interrupts
}



void handle_rxspecial_int(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine handles Rx Special interrupt.

Arguments:

	sync_port - points to port driver structure.

Return Value:

	None.

--*/
{
	unsigned char ch;
	unsigned char  rx_special_stat;
	struct _api_context *api = &sync_port->api;
	unsigned short frame_len=0;
	
	rx_special_stat = api->read_reg_char (sync_port->escc.reg_address+REG1);

	//printk("rx_special_stat %x\n",rx_special_stat);

	if(rx_special_stat & FRAME_END_SDLC)
	{
		ch = api->read_reg_char (sync_port->escc.reg_address+REG6);
		frame_len = ((api->read_reg_char (sync_port->escc.reg_address+REG6) << 8) | ch) & 0x3f;		
		if ((rx_special_stat & FRAME_CRC_ERROR) && sync_port->port_settings.crc.rx_type)
		{
			sync_port->events |= EVENT_FE;			
		}
		if (!sync_port->escc.flag_comsync_III) 
		{		
			if(sync_port->escc.use_dma_read)
			{
				if ( sync_port->escc.read_dma_busy)
					dma_rx_dpc(sync_port, 1);
			}
			else if(sync_port->port_settings.crc.rx_type)
			{
				rx_resume(sync_port, NULL);
			}
		}
	}
	
	if (!sync_port->escc.flag_comsync_III) 	
	{
		if(rx_special_stat == (ALL_SENT_ESCC|RESIDUE_2|RESIDUE_1))
		{
			rx_resume(sync_port, NULL);
		}
		else
		{
	  		// Mask the residue bits
	  		unsigned char residuebits = rx_special_stat & 0x0E;
	  		// If the residue bits are different from the reset state
	  		if (residuebits != (RESIDUE_2|RESIDUE_1))
	  		{
	        	// unlock the FIFO
	        	do {
	               //ERROR_RESET first
	               api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_ERROR);
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
	              rx_push_data(sync_port, &ch);
	        	}while (api->read_reg_char(sync_port->escc.reg_address+REG0) & RX_AVAIL_ESCC);
	    	}
		}
	}

	if(rx_special_stat & (PARITY_ERROR|OVERRUN_ERROR))
	{
		if( rx_special_stat & PARITY_ERROR)
		{
			//printk("Parity error\n");
			sync_port->events |= EVENT_PE;
		}
		if(rx_special_stat & OVERRUN_ERROR)
		{
			//printk("RxOverrun error\n");
			sync_port->events |= EVENT_OE;
		}
		if (sync_port->escc.flag_comsync_III) 
			goto err_exit;
		// unlock the FIFO
		do {
			//ERROR_RESET first
			api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_ERROR);
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
			rx_push_data(sync_port, &ch);
		}while (api->read_reg_char(sync_port->escc.reg_address+REG0) & RX_AVAIL_ESCC);
	}
err_exit:
	//ERROR_RESET;
	api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_ERROR);
}

void comsyncIII_transmit_dma(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine will write data into DMA buffer to be transmitted by DMA engine.

Arguments:

	sync_port - points to port driver structure.

	frame_len - frame length
	
	stat -	 frame status such as CRC 


Return Value:

	None.

--*/
{			
	//wait for the CRC/flag generation, it should be done automatically as it is set in WR7p auto_eom_reset, but it misses sometimes in high bitrate	
	if(sync_port->port_settings.smode.rx != SMODE_ASYNC)
	{
		if(sync_port->port_settings.crc.tx_type)
			while(!(sync_port->api.read_reg_char (sync_port->escc.reg_address+REG0) & 0x40));
	}
	
	
	sync_port->api.write_reg_long ((void *)sync_port->tx.dma_list_addr[sync_port->tx.dma_buf_index], sync_port->tx.dma_phy_addr[sync_port->tx.dma_addr_buf_index]);
	sync_port->api.write_reg_long ((void *)sync_port->tx.dma_count_addr[sync_port->tx.dma_buf_index], sync_port->tx.len);			
	
	sync_port->tx.dma_buf_index = sync_port->tx.dma_buf_index ? 0:1;					
	sync_port->tx.dma_addr_buf_index++;
	if(sync_port->tx.dma_addr_buf_index > 2)
		sync_port->tx.dma_addr_buf_index = 0;		
	sync_port->tx.len = 0;
	sync_port->tx.tx_int_wait = 1;			
	
}

void comsyncIII_receive_dma(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine will read data from DMA buffer and initialize DMA list if needed.

Arguments:

	sync_port - points to port driver structure.

	frame_len - frame length
	
	stat -	 frame status such as CRC 


Return Value:

	None.

--*/
{
	unsigned char  ch;	
	unsigned long count;
	int j;
	struct _api_context *api = &sync_port->api;	
	int index = sync_port->rx.dma_buf_index;
						
	count = sync_port->rx.dma_count - api->read_reg_long ((void *)sync_port->rx.dma_count_addr[index]);
	api->write_reg_long (sync_port->escc.base_address + 0xa4, RELEASE_RX_HALT(sync_port->escc.index));			
	
	for(j=0; j<count; j++)
	{
		ch = sync_port->rx.dma_buffer[index][j+sync_port->rx.dma_buf_cur_index];		
		rx_push_data(sync_port, &ch);
	}
	
	sync_port->rx.dma_count -= count;
	sync_port->rx.dma_buf_cur_index += count;
	if(sync_port->rx.dma_buf_cur_index >= DMA_BUF_REQEST)
	{
		sync_port->rx.dma_count = DMA_BUF_REQEST;
		sync_port->rx.dma_buf_cur_index = 0;
		
		api->write_reg_long ((void *)sync_port->rx.dma_list_addr[index], sync_port->rx.dma_phy_addr[index]);
		api->write_reg_long ((void *)sync_port->rx.dma_count_addr[index], DMA_BUF_REQEST);
		
		sync_port->rx.dma_buf_index = index ? 0:1;
	}
}

void sdlc_rx_push_info(
	struct sync_port_drv_struct*  sync_port,
	unsigned short                frame_len,
	unsigned char 						stat
	)
/*++

Routine Description:

	This routine will push SDLC frame length and status into circular buffer.

Arguments:

	sync_port - points to port driver structure.

	frame_len - frame length
	
	stat -	 frame status such as CRC 


Return Value:

	None.

--*/
{
	struct rx_struct*     rx = &(sync_port->rx);
	unsigned long         slots_free;

	slots_free = CIRC_BUF_EMPTY_AVAIL(
		rx->sdlc_frames.head,
		rx->sdlc_frames.tail,
		BUF_SIZE);
	
	if (slots_free)
	{
		rx->sdlc_frames.info[rx->sdlc_frames.head].len  =  frame_len;
		rx->sdlc_frames.info[rx->sdlc_frames.head].stat =  stat;
		rx->sdlc_frames.head++;
				
		if(rx->sdlc_frames.head == BUF_SIZE)
			rx->sdlc_frames.head = 0;
	}
}

void sdlc_receive_int(struct sync_port_drv_struct *sync_port)
/*++

Routine Description:

	This routine handles SDLC fifo status interrupt that comes through receive interrupt.

Arguments:

	sync_port - points to port driver structure.

	
Return Value:

	None.

--*/
{
	unsigned char reg;
	unsigned short frame_len=0;
	
	reg = sync_port->api.read_reg_char (sync_port->escc.reg_address+REG6);
	frame_len = ((sync_port->api.read_reg_char (sync_port->escc.reg_address+REG7) << 8) | reg) & 0x3fff;
	reg = sync_port->api.read_reg_char (sync_port->escc.reg_address+REG1);
	
	sync_port->api.write_reg_char (sync_port->escc.reg_address+REG0, RESET_HIGHEST_IUS);
	sdlc_rx_push_info(sync_port, frame_len, reg);
	comsyncIII_receive_dma(sync_port);	
	sync_port->rx.wakeup = 1;
	wake_up_interruptible(&sync_port->rx.wait);
}

int comsyncIII_int(struct bus_driver_struct *bus_driver_context)
/*++

Routine Description:

	This is the entry routine for Comsync-III interrupt.

Arguments:

	bus_driver_context - pointer bus_driver_struct structure.	

Return Value:

	None.

--*/
{
	unsigned char  reg_ch;	
	unsigned long reg_l, master_int_stat, dma_int_stat, dma_stat;
	struct sync_port_drv_struct *sync_port;
	struct _api_context *api = &bus_driver_context->api;	
	int i;
	
	
	master_int_stat =  api->read_reg_char (bus_driver_context->escc_base_address + 0xa0);
	if(master_int_stat & ESCC_INTR)
	{
		reg_ch = api->read_reg_char (bus_driver_context->escc_base_address + 0x32);
		reg_ch =	(reg_ch >> 1) & 0x07;		
		
		if (reg_ch == 0) {		
			sync_port = &bus_driver_context->sync_port_driver[CH_B];
			handle_tx_int(sync_port);
		}
		if (reg_ch == 1) {		
			sync_port = &bus_driver_context->sync_port_driver[CH_B];
			handle_ext_int(sync_port);
		}	
		if (reg_ch == 2) {								
			sync_port = &bus_driver_context->sync_port_driver[CH_B];
			if (sync_port->port_settings.smode.rx == SMODE_HDLC)
				sdlc_receive_int(sync_port);
			//we should not get this interrupt for any other mode
			
		}
		if (reg_ch == 3) {		
			sync_port = &bus_driver_context->sync_port_driver[CH_B];
			handle_rxspecial_int(sync_port);
		}
		if (reg_ch == 4) {	
			sync_port = &bus_driver_context->sync_port_driver[CH_A];
			handle_tx_int(sync_port);
		}
		if (reg_ch == 5) {	
			sync_port = &bus_driver_context->sync_port_driver[CH_A];
			handle_ext_int(sync_port);
		}	
		if (reg_ch == 6) {													
			sync_port = &bus_driver_context->sync_port_driver[CH_A];
			if (sync_port->port_settings.smode.rx == SMODE_HDLC)
				sdlc_receive_int(sync_port);	
			//we should not get this interrupt for any other mode		
		}
		if (reg_ch == 7) {		
			sync_port = &bus_driver_context->sync_port_driver[CH_A];
			handle_rxspecial_int(sync_port);
		}		
	}
	
	if(master_int_stat & DMA_INTR)
	{
		dma_stat = api->read_reg_long (bus_driver_context->escc_base_address + 0x84);
		dma_int_stat = api->read_reg_long (bus_driver_context->escc_base_address + 0x88);
		// clear the interrupt first
		api->write_reg_long (bus_driver_context->escc_base_address + 0x88, dma_int_stat & 0x000000ff);
		
		for(i=0; i<NUM_COMSYNC_PORT; i++)
		{
			sync_port = &bus_driver_context->sync_port_driver[i];			
			if(dma_int_stat & TX_EOB_EOL(i))
			{						
				//printk("%d:TX_EOB_EOL  \n",i);				
				if (sync_port->tx.len && sync_port->port_settings.smode.rx != SMODE_HDLC) 		
					comsyncIII_transmit_dma(sync_port);
				sync_port->tx.wakeup = 1;				
				wake_up_interruptible(&sync_port->tx.wait);
			}
			
			if(dma_int_stat & RX_EOL(i))
			{	
				//printk("%d:RX_EOB_EOL  \n",i);		
				comsyncIII_receive_dma(sync_port);	
				if(!(dma_stat & RX_BUF_BUSY(i, !sync_port->rx.dma_buf_index)))
				{					
					comsyncIII_receive_dma(sync_port);
				}		
				if (sync_port->port_settings.smode.rx != SMODE_HDLC)
				{	
					sync_port->rx.wakeup = 1;
					wake_up_interruptible(&sync_port->rx.wait);				
				}
			}
			else if(dma_int_stat & RX_EOB(i))
			{		
				//printk("%d:RX_EOB  \n",i);										
				comsyncIII_receive_dma(sync_port);
				if (sync_port->port_settings.smode.rx != SMODE_HDLC)
				{
					sync_port->rx.wakeup = 1;
					wake_up_interruptible(&sync_port->rx.wait);		
				}
			}													
		}				
	}
	
	
	if(master_int_stat & FIFO_INTR)
	{		
		reg_l = api->read_reg_char(bus_driver_context->escc_base_address + 0xa4);
		api->write_reg_char(bus_driver_context->escc_base_address + 0xa4, reg_l);
		//printk("FIFO_INTR %x\n",reg_l);
		for(i=0; i<NUM_COMSYNC_PORT; i++)
		{
			sync_port = &bus_driver_context->sync_port_driver[i];	
			if(reg_l & RX_ENE_TO_EVENT(i))
			{							
				comsyncIII_receive_dma(sync_port);
				if (sync_port->port_settings.smode.rx != SMODE_HDLC)
				{
					sync_port->rx.wakeup = 1;
					wake_up_interruptible(&sync_port->rx.wait);												
				}
			}			
		}
	}
	
	/**
	**	exit ISR handler
	**/
	
	reg_l = bus_driver_context->api.read_reg_char(bus_driver_context->comsyncIII_bar2 + 0x5d);
	bus_driver_context->api.write_reg_char(bus_driver_context->comsyncIII_bar2 + 0x5d, BACKEND_INT_STAT|(reg_l & 0xf1));				
	
	return IRQ_HANDLED;
}

int escc_interrupt_handler(int irq, void  *context)
/*++

Routine Description:

    This is the interrupt service routine for the Comsync/104 port driver.


Arguments:

    irq - IRQ number

    Context - This is really a pointer to bus_driver_struct.

Return Value:

    This function will return IRQ_HANDLED.

--*/

{
	struct bus_driver_struct *bus_driver_context = (struct bus_driver_struct *)context;
	unsigned char TcInt;
	unsigned char vector;	
	struct sync_port_drv_struct *sync_port;
	struct _api_context *api = &bus_driver_context->api;	
	

	
	//printk("ComSyncISR-104\n");	
	
	if(bus_driver_context->sync_port_driver[0].escc.flag_comsync_III)
		return comsyncIII_int(bus_driver_context);

begin:	
		
	TcInt = api->read_reg_char (bus_driver_context->escc_base_address + CS104_CtrlStat);
	//printk("TcInt %x\n",TcInt);

	if ((TcInt  & 0x28) == 0x08)
	{
		return IRQ_HANDLED;
	}
	// is it TC int?
	if (TcInt & 0x20)
	{
		if (bus_driver_context->sync_port_driver[CH_A].escc.write_dma_busy)
		{
			if (bus_driver_context->sync_port_driver[CH_A].port_settings.async_settings.hflow == LSET_HFLOW_RTS_TOGGLE)
			{
				bus_driver_context->sync_port_driver[CH_A].escc.WriteReg.r5.wr5.bf.rts = 0;
				api->write_reg_char(bus_driver_context->sync_port_driver[CH_A].escc.reg_address+REG5,bus_driver_context->sync_port_driver[CH_A].escc.WriteReg.r5.wr5.reg);
			}

			dma_tx_dpc(&bus_driver_context->sync_port_driver[CH_A]);
		}

		if (bus_driver_context->sync_port_driver[CH_B].escc.write_dma_busy)
		{
			if (bus_driver_context->sync_port_driver[CH_B].port_settings.async_settings.hflow == LSET_HFLOW_RTS_TOGGLE)
			{
				bus_driver_context->sync_port_driver[CH_B].escc.WriteReg.r5.wr5.bf.rts = 0;
				api->write_reg_char(bus_driver_context->sync_port_driver[CH_B].escc.reg_address+REG5,bus_driver_context->sync_port_driver[CH_B].escc.WriteReg.r5.wr5.reg);
			}

			dma_tx_dpc(&bus_driver_context->sync_port_driver[CH_B]);
		}

		if (bus_driver_context->sync_port_driver[CH_A].escc.read_dma_busy)
		{
			dma_rx_dpc(&bus_driver_context->sync_port_driver[CH_A], 0);
		}

		if (bus_driver_context->sync_port_driver[CH_B].escc.read_dma_busy)
		{
			dma_rx_dpc(&bus_driver_context->sync_port_driver[CH_B], 0);
		}

		// clear TC INT
		//printk("Clear Tc INT\n");
		api->write_reg_char(bus_driver_context->escc_base_address + CS104_TC_Secur,0xff);
	}


	if ((TcInt  & 0x08))
	{
		goto begin;
	}			
	
	
	/**
	**	obtain vector from SCC
	**	(Use special INT Acknowledge cycle provided by card)
	**/

	vector = api->read_reg_char (bus_driver_context->escc_base_address + CS104_RST_IACK);
	if (vector & 0x08) {
		sync_port = &bus_driver_context->sync_port_driver[CH_A];
	}
	else
	{		
		sync_port = &bus_driver_context->sync_port_driver[CH_B];
	}
	
	
	/**
	**	find out what needs service
	**	TX, RX, RX-Special, EXT-Stat
	**/
	vector &= 0x06;			// mask off unneeded bits

	switch (vector) {
	case 0:
			/**
			**	TX service
			**/

		handle_tx_int(sync_port);

		break;

	case 2:
			/**
			**	EXT-Stat service
			**/
		handle_ext_int(sync_port);
		
		break;

	case 4:
		/**
		**	RX service
		**/
		handle_rx_int(sync_port);

		break;

	case 6:
		/**
		**	RX_Special service
		**/
		handle_rxspecial_int(sync_port);

		break;
	}	

	/**
	**	Reset Highest IUS on ESCC (wr0)
	**/
	api->write_reg_char (sync_port->escc.reg_address+REG0, RESET_HIGHEST_IUS);
	
	/**
	**	exit ISR handler
	**/	

	return IRQ_HANDLED;
}
