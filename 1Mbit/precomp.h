/*******************************************************************************
**	File:			precomp.h
**	Author:			Hasan Jamal
**
**
**	Description:
**		header file inclusion place
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

#ifndef _comsync_precomp_h
	#define _comsync_precomp_h

#include <linux/dma-mapping.h>
#include <asm/dma.h>
#include<linux/version.h>

#include "CSPCI_hw.h"
#include "CSPCI_pset.h"
#include "plx9054.h"
#include "CSPCI_iusc.h"
#include "osapi.h"
#include "scc.h"
#include "prf512_hw.h"
#include "cs104.h"
#include "drv_struct.h"
#include "debug.h"
#include "ioctls.h"


void tx_resume(struct sync_port_drv_struct*  sync_port, unsigned short *fifo_avail);
void rx_resume(	struct sync_port_drv_struct*  sync_port, unsigned short* rx_avail);
void rx_push_data(	struct sync_port_drv_struct*  sync_port, unsigned char* data);
void reset_escc_ch(struct sync_port_drv_struct *sync_port);
int escc_dma_cancel(int channel);
void start_dma_dtr_req( struct escc_struct *escc, struct _api_context *api);
void stop_dma_dtr_req(struct escc_struct *escc, struct _api_context *api);
void start_dma_wreq_read( struct escc_struct *escc, struct _api_context *api);
void stop_dma_wreq_read(struct escc_struct *escc, struct _api_context *api);
int escc_interrupt_handler(int irq, void  *context);
void start_DMA_read(struct sync_port_drv_struct *sync_port);
int escc_dma_isdone(int channel);
int setup_escc(BHN_PortSettings *set, struct escc_struct *escc, struct _api_context *api);
void escc_set_rts(struct escc_struct *escc, struct _api_context *api, char set_reset);
void escc_set_dtr(struct escc_struct *escc, struct _api_context *api, char set_reset);
void escc_send_break(struct escc_struct *escc, struct _api_context *api, char set_reset);
void comsyncIII_transmit_dma(struct sync_port_drv_struct *sync_port);

#endif


