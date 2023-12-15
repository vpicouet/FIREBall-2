/*******************************************************************************
**	File:			isr.c
**	Author:			Rod Dore, Hasan Jamal
**
**
**	Description:
**		General purpose ISR for IUSC
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

#include "precomp.h"


static void tx_stat(
	struct sync_port_drv_struct*  sync_port
	)
/*++

Routine Description:

    this routine does TX Status Handling

Arguments:

    sync_port - points to port driver structure.

Return Value:

	None

--*/
{
	unsigned short     tx_stat;
	unsigned char      handled = 0;
	unsigned char      ticr;
	unsigned char      new_ia = 0;
	unsigned char      end_rts_toggle = 0;

	tx_stat = sync_port->api.read_reg_word((void*)&sync_port->iusc->TCSR.WORD);
	ticr = sync_port->api.read_reg_char((void*)&sync_port->iusc->TICR.byte.LSB) & TICR_ALL_IA;

	DBGPRINT(DEBUG_TXSTAT, KERN_INFO, "[%u]TX status 0x%x 0x%x (",
		sync_port->index, tx_stat, ticr);

	if ((tx_stat & PRE_SENT) && (ticr & PRE_SENT_IA)) {
		handled |= PRE_SENT;
		DBGPRINTNOHEAD(DEBUG_TXSTAT, "PRE_SENT ");
		sync_port->events |= EVENT_PRE_SENT;

	}

	if ((tx_stat & ABORT_SENT) && (ticr & ABORT_SENT_IA)) {
		handled |= ABORT_SENT;
		DBGPRINTNOHEAD(DEBUG_TXSTAT, "ABORT_SENT ");
		sync_port->events |= EVENT_ABORT_SENT;

	}

	if ((tx_stat & EOFM_SENT) && (ticr & EOFM_SENT_IA)) {
		handled |= EOFM_SENT;
		DBGPRINTNOHEAD(DEBUG_TXSTAT, "EOFM_SENT ");
		sync_port->events |= EVENT_EOFM_SENT;

		if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE) {
			/* unlatch and arm the IDLE_SENT interrupt */
			//handled |= IDLE_SENT;
			new_ia |= IDLE_SENT_IA;
			if (tx_stat & IDLE_SENT)
				end_rts_toggle = 1;
		}
	}

	if ((tx_stat & TX_UNDER) && (ticr & TX_UNDER_IA)) {
		handled |= TX_UNDER;
		DBGPRINTNOHEAD(DEBUG_TXSTAT, "TX_UNDER ");
		sync_port->events |= EVENT_TX_UNDER;

		if (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE) {
			/* unlatch and arm the IDLE_SENT interrupt */
			//handled |= IDLE_SENT;
			new_ia |= IDLE_SENT_IA;
			if (tx_stat & IDLE_SENT)
				end_rts_toggle = 1;
		}
	}

	if (tx_stat & IDLE_SENT) {
		handled |= IDLE_SENT;
		if (ticr & IDLE_SENT_IA) {
			DBGPRINTNOHEAD(DEBUG_TXSTAT, "IDLE_SENT ");
			sync_port->events |= EVENT_IDLE_SENT;
			end_rts_toggle = 1;
		}
	}

	if (end_rts_toggle && (sync_port->port_settings.async_settings.hflow & LSET_HFLOW_RTS_TOGGLE)) {
		iusc_set_rts((void*)sync_port->iusc, &sync_port->api, 0);
		new_ia &= ~IDLE_SENT_IA;
	}

	if ((tx_stat & CRC_SENT) && (ticr & CRC_SENT_IA)) {
		handled |= CRC_SENT;
		DBGPRINTNOHEAD(DEBUG_TXSTAT, "CRC_SENT ");
		sync_port->events |= EVENT_CRC_SENT;

	}

	DBGPRINTNOHEAD(DEBUG_TXSTAT, ")\n");


	/* unlatch the handled TCSR bits by writting 1 back to them */
	sync_port->api.write_reg_char((void*)&sync_port->iusc->TCSR.byte.LSB, handled);

	/* Re-arm all the IA bits, so that future interrupts will occur
	 * (See IUSC manual page 7-14 and 7-27)
	 */
	WRITE_TICR_LSB(sync_port->iusc, 0,  &sync_port->api);
	WRITE_TICR_LSB(sync_port->iusc, (ticr & ~IDLE_SENT_IA) | new_ia,  &sync_port->api);
}


static void rx_stat(
	struct sync_port_drv_struct*  sync_port
	)
/*++

Routine Description:

    this routine does RX Status Handling

Arguments:

    sync_port - points to port driver structure.

Return Value:

	None

--*/
{
	unsigned short     rx_stat;
	unsigned char      ricr;
	unsigned char      handled = 0;


	rx_stat = sync_port->api.read_reg_word((void*)&sync_port->iusc->RCSR.WORD);
	ricr = sync_port->api.read_reg_char((void*)&sync_port->iusc->RICR.byte.LSB);

	DBGPRINT(DEBUG_RXSTAT, KERN_INFO,
		"[%u]RX stat 0x%x 0x%x\n", sync_port->index, rx_stat, ricr);

	if ((rx_stat & EXITED_HUNT) && (ricr & EXITED_HUNT_IA)) {
		handled |= EXITED_HUNT;
		/* at the moment there's no event for this */
	}

	if ((rx_stat & IDLE_RECV) && (ricr & IDLE_RECV_IA)) {
		handled |= IDLE_RECV;
		sync_port->events |= EVENT_IDLE_RECVD;
	}

	if ((rx_stat & BREAK_ABORT_RECV) && (ricr & BREAK_ABORT_RECV_IA)) {
		handled |= BREAK_ABORT_RECV;
		if (sync_port->port_settings.smode.rx == SMODE_ASYNC) {
			sync_port->events |= EVENT_BREAK;
		} else {
			sync_port->events |= EVENT_ABORT;
		}
	}

	/* unlatch the handled bits */
	sync_port->api.write_reg_char((void*)&sync_port->iusc->RCSR.byte.LSB, handled);
}

void misc_stat(
	struct sync_port_drv_struct*  sync_port,
	unsigned short reg_sicr,
	unsigned short reg_misr
	)
/*++

Routine Description:

    this routine does micellaneous Status Handling

Arguments:

    sync_port - points to port driver structure.

    reg_sicr - SICR register content

    reg_misr - MISR regsiter content

Return Value:

	None

--*/
{
	DBGPRINT(DEBUG_MISCSTAT, KERN_INFO, "[%u]MISC status 0x%x 0x%x (",
		sync_port->index, reg_sicr, reg_misr);

	if ((reg_sicr & RCC_UNDER_IA) && (reg_misr & RCC_UNDER_LU)) {
		sync_port->events |= EVENT_RCC_UNDER;
	}
	if ((reg_sicr & DPLL_SYNC_IA) && (reg_misr & DPLL_SYNC_LU)) {
		sync_port->events |= EVENT_DPLL_DSYNC;
	}
}


void io_stat(
	struct sync_port_drv_struct*  sync_port,
	unsigned short reg_sicr,
	unsigned short reg_misr
	)
/*++

Routine Description:

    this routine does I/O Status Handling

Arguments:

    sync_port - points to port driver structure.

    reg_sicr - SICR register content

    reg_misr - MISR regsiter content

Return Value:

	None

--*/
{
	DBGPRINT(DEBUG_IOSTAT, KERN_INFO, "[%u]MISC status 0x%x 0x%x (",
		sync_port->index, reg_sicr, reg_misr);

	if (reg_sicr & RXR_IA) {
		// Check RI pin state change
		if (reg_misr & RXR_LU) {
			// An enabled transition occured
			if (reg_misr & RXR_IS) {
				sync_port->events &= ~EVENT_RI_FALL;
				sync_port->events |= EVENT_RI_RISE;

			} else {
				sync_port->events &= ~EVENT_RI_RISE;
				sync_port->events |= EVENT_RI_FALL;
			}
		}
		/* else, transition has not occured */
	}

	if (reg_sicr & TXR_IA) {
		// DSR pin state change
		if (reg_misr & TXR_LU) {
			// An enabled transition occured
			if (reg_misr & TXR_IS) {
				sync_port->events &= ~EVENT_DSR_FALL;
				sync_port->events |= EVENT_DSR_RISE;
			} else {
				sync_port->events &= ~EVENT_DSR_RISE;
				sync_port->events |= EVENT_DSR_FALL;
			}
		}
		/* else, transition has not occured */
	}

	if (reg_sicr & DCD_IA) {
		//	DCD pin state change
		if (reg_misr & DCD_LU) {
			// An enabled transition occured
			if (reg_misr & DCD_IS) {
				sync_port->events &= ~EVENT_DCD_FALL;
				sync_port->events |= EVENT_DCD_RISE;
			} else {
				sync_port->events &= ~EVENT_DCD_RISE;
				sync_port->events |= EVENT_DCD_FALL;
			}
		}

		/* else, transition has not occured */
	}

	if (reg_sicr & CTS_IA) {
		// CTS pin state change
		if (reg_misr & CTS_LU) {
			// An enabled transition occured
			if (reg_misr & CTS_IS) {
				sync_port->events &= ~EVENT_CTS_FALL;
				sync_port->events |= EVENT_CTS_RISE;
			} else {
				sync_port->events &= ~EVENT_CTS_RISE;
				sync_port->events |= EVENT_CTS_FALL;
			}
		}
		/* else, transition has not occured */
	}
}



void iusc_isr_handler (void *context)

/*
 * Interrupt Handler for the IUSC
 *
 * Arg:
 *     context - pointer to struct bus_driver_struct
 */
{
	unsigned char pld_int_status, iusc_vector, isr_type;
	unsigned short temp_reg1, temp_reg2;
	int port;
	struct sync_port_drv_struct *sync_port;
	unsigned short tx_space;
	unsigned short rx_avail;
	struct bus_driver_struct *bus_driver_context = (struct bus_driver_struct *)context;
	struct _api_context *api = &bus_driver_context->api;
	volatile _main_pld *pld = bus_driver_context->b_pld;

	// get the initial state of all IUSC chip interrupts
	pld_int_status = api->read_reg_word((void*)&pld->pld_status) & 0x0003;

	do {
		// Handle all IUSC's that are interrupting at this point in time
		while (pld_int_status) {
			DBGPRINT(DEBUG_INTR, KERN_INFO, "IUSC int; 0x%x\n", pld_int_status);

			// Scan all ports (one per IUSC device)
			for (port=0; port< N_SYNC_PORTS; port++) {
				sync_port = &bus_driver_context->sync_port_driver[port];

				// check the INT bit for this port
				if (pld_int_status & PN_IRQ(port)) {

					// setup pointer to the correct IUSC device
					//iusc = MK_FP(IuscPld_Seg, IUSC_OFFSET(port));
					// perform an Interrupt Acknowledge Read to obtain the vector from the device
					iusc_vector = api->read_reg_word((void*)&sync_port->iusc->IntAck);
					DBGPRINT(DEBUG_INTR, KERN_INFO,
							"[%d]serial int iusc_vector %x\n", port,iusc_vector);

					// Test the vector as being "Serial" or "DMA"
					if (iusc_vector & SERIAL_VECTOR) {
						DBGPRINT(DEBUG_INTR, KERN_INFO,
							"[%d]serial int\n", port);

						// Serial type, get the ISR type from the vector
						isr_type = GET_SER_TYPE(iusc_vector);
						switch (isr_type) {

							//	No Serial Interrupt pending
							case SER_NO_PEND:
								//	This condition might happen if the IUSC device clears
								//	the interrupt signal too slowly.
								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->no pending\n", port);
								break;

							//	Misc Interrupt Sources
							case SER_MISC_PEND:
								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->misc\n", port);
								// Clear the IP bit
								api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | MISC_IP));
								// Get the Interrupt Arm and Status bits
								temp_reg1 = api->read_reg_word((void*)&sync_port->iusc->SICR.WORD);		// these bits indicate which interrupts are "armed"
								temp_reg2 = api->read_reg_word((void*)&sync_port->iusc->MISR.WORD);		// get misc interrupt status

								misc_stat(sync_port, temp_reg1, temp_reg2);
								// The RxC and TxC pin state changes and BRG interrupts are not allowed,
								//	see 'iusc_int_setup' function above.

								// Unlatch all MISC interrupt conditions
								api->write_reg_word((void*)&sync_port->iusc->MISR.WORD, ALL_MISC_LU);
								//	Clear IUS bit (this is done later)
								break;

							//	I/O pin Interrupt Sources
							case SER_IO_PEND:
								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->I/O\n", port);
								// Clear the IP bit
								api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | IOP_IP));
								temp_reg1 = api->read_reg_word((void*)&sync_port->iusc->SICR.WORD);		// these bits indicate which interrupts are "armed"
								temp_reg2 = api->read_reg_word((void*)&sync_port->iusc->MISR.WORD);		// get misc interrupt status

								io_stat(sync_port, temp_reg1, temp_reg2);
								// Unlatch all I/O interrupt conditions
								api->write_reg_word((void*)&sync_port->iusc->MISR.WORD, ALL_IO_LU);
								//	Clear IUS bit (this is done later)
								break;

							//	Transmit Data Interrupt
							case SER_TX_DATA_PEND:

								// Clear the IP bit
								api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | TD_IP));

								//	How much space is available?
								/*CJD20080731tx_space = iusc_get_tx_space(port);*/
								//	Since the IUSC setup leaves the TCSR register command
								//		set to SEL_TXFIFO_STAT, it quicker to just get the
								//		TX Fifo free space by just reading the TICR.MSB
								tx_space = api->read_reg_char((void*)&sync_port->iusc->TICR.byte.MSB);//iusc_get_tx_space((void*)iusc, (void*)(api));

								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->TX data tx_space %d\n", port,tx_space);
								tx_resume(sync_port, &tx_space);

								break;

							//	Transmitter Status Interrupt
							case SER_TX_STAT_PEND:
								DBGPRINT(DEBUG_INTR | DEBUG_TXSTAT, KERN_INFO,
									"[%d]si->TX stat\n", port);
								/* clear the IP bit */
								api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | TS_IP));

								/* tx_stat() will unlatch and re-arm as
								 * appropriate
								 */
								tx_stat(sync_port);
								break;

							//	Receiver Data Interrupt
							case SER_RX_DATA_PEND:
								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->RX data\n", port);

								rx_avail = iusc_get_rx_avail((void*)sync_port->iusc, (void*)(api));
								if(rx_avail == 0)
								{
									// Clear the IP bit
									api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | RD_IP));
									rx_avail = iusc_get_rx_avail((void*)sync_port->iusc, (void*)(api));
								}
								while (rx_avail) {
									//check for FIFO overrun
									if(rx_avail > FIFO_SIZE)
										rx_avail = FIFO_SIZE;

									rx_resume(sync_port, &rx_avail);


									if (!rx_avail) {
										// Clear the IP bit
										api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | RD_IP));

										/* re-read FIFO count to avoid a lost interrupt */
										if (sync_port->port_settings.smode.rx != SMODE_ASYNC) {
											/* if we re-read the FIFO count in ASYNC, we end up
											 * generating many small little directors, causing
											 * poor performance
											 * instead, in ASYNC, we let the timer scoop collect any data
											 * that may be stuck due to a lost interrupt
											 */
											rx_avail = iusc_get_rx_avail((void*)sync_port->iusc, (void*)(api));
										}
									}
								}
								break;

							//	Receiver Status Interrupt
							case SER_RX_STAT_PEND:
							{
								unsigned char temp_reg;
								//CJD20090116struct stat_packet_struct stat_packet; // RCSR packet stat structure

								DBGPRINT(DEBUG_INTR, KERN_INFO,
									"[%d]si->RX stat\n", port);

								// Clear the IP bit
								api->write_reg_word((void*)&sync_port->iusc->DCCR.WORD, (CLEAR_IP | RS_IP));

								rx_stat(sync_port);

								//	Re-arm all the IA bits, so that future interrupts will occur
								//	(See IUSC manual page 7-14 and 7-27)
								temp_reg = api->read_reg_char((void*)&sync_port->iusc->RICR.byte.LSB) & RICR_ALL_IA;	//save the IA bits which need to be restored
								WRITE_RICR_LSB(sync_port->iusc, 0, api);
								WRITE_RICR_LSB(sync_port->iusc, temp_reg, api);
							}
							break;

							// Others ?? This should not happen
							default:
								goto next_port;		//	see below
						}	//switch (isr_type)

						//	Reset the highest priority IUS bit
						DO_CCAR_CMD(sync_port->iusc, RST_HIGHEST_SERIAL_IUS, api);

					}	//if

					else {
						DBGPRINT(DEBUG_INTR, KERN_INFO,
							"[%d]DMA int\n", port);
						// Must be a DMA vector
						isr_type = GET_DMA_TYPE(iusc_vector);
						switch (isr_type) {

							case DMA_NO_PEND:
								//	This condition might happen if the IUSC device clears
								//	the interrupt signal too slowly.
								break;

							case DMA_TX_EVENT:
								// Clear the IP bit
								//iusc->CDIR.WORD = CLR_TXDMA_IP;
								api->write_reg_word((void*)&sync_port->iusc->CDIR.WORD, CLR_TXDMA_IP);
								//	Read the RX DMA Status & clear the bits that caused the interrupt
								temp_reg1 = api->read_reg_word((void*)&sync_port->iusc->TDMR.WORD) & TR_DMA_STATUS;
								if (temp_reg1 & EOA_EOL) {
								}
								if (temp_reg1 & EOB) {
								}
								if (temp_reg1 & HABORT) {
								}
								if (temp_reg1 & SABORT) {
								}


								break;

							case DMA_RX_EVENT:
								// Clear the IP bit
								//iusc->CDIR.WORD = CLR_RXDMA_IP;
								api->write_reg_word((void*)&sync_port->iusc->CDIR.WORD, CLR_RXDMA_IP);
								//	Read the RX DMA Status & clear the bits that caused the interrupt
								temp_reg1 = api->read_reg_word((void*)&sync_port->iusc->RDMR.WORD) & TR_DMA_STATUS;
								if (temp_reg1 & EOA_EOL) {
								}
								if (temp_reg1 & EOB) {
								}
								if (temp_reg1 & HABORT) {
								}
								if (temp_reg1 & SABORT) {
								}


								break;

							// Others ?? This should not happen
							default:
								goto next_port;		//	see below
						}	//switch

						//	Reset the highest priority DMA IUS bit
						api->write_reg_word((void*)&sync_port->iusc->DCAR.WORD, RESET_HIGHEST_DMA_IUS | MBRE);
					}	//else
				}	//if (pld_int_status)

				//	remove this port from the status value
				next_port:
				if ( (pld_int_status &= ~(PN_IRQ(port))) == 0 ) goto check_status_again;
			}	//for (port)
		}	//while (pld_int_status)

		// snapshot the interrupt bits again
		check_status_again:
		//pld_int_status = pld->pld_status & 0x0003;
		pld_int_status = api->read_reg_word((void*)&pld->pld_status)& 0x0003;

	} while (pld_int_status);
}
