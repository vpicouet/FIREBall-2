/************************************************************
**	File:	escc.c
**	Author:
**			Rod Dore, Hasan Jamal
**
**	Description:
**		This module contains the code that is very specific to ESCC
**		operations in the Comsync driver
**
**	Revision history:
**		Original:	11/23/10
**
**	THIS IS THE UNPUBLISHED PROPRIETARY SOURCE CODE OF CONNECT TECH INC.
**	The copyright notice above does not evidence any actual or intended
**	publication of such source code.
**
**	This module contains Proprietary Information of Connect Tech, Inc
**	and should be treated as Confidential.
************************************************************/

#include "precomp.h"


int setup_escc(BHN_PortSettings *set, struct escc_struct *escc, struct _api_context *api)

/**************************************************************************

	Setup a Port (ESCC device and surrounding circuits)
		Args:
			'set'			a pointer to BHN_PortSettings structure
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
		Return:
			0	=	success
			>0	=	failure code (see codes at top of this file)
**************************************************************************/
{

	unsigned long divisor;
	long konst;
	unsigned char reg;


	// line interface mode
	reg = api->read_reg_char ((void *)escc->base_address + CS104_Misc);
	if (set->line_mode > LM_V28)
		return BAD_line_mode;
	switch(set->line_mode)
	{
	case V11:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=V11);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=V11<<3);
		break;

	case LM_530A:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=EIA530A);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=EIA530A<<3);
		break;

	case LM_530:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=EIA530);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=EIA530<<3);
		break;

	case LM_X21:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=X21);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=X21<<3);
		break;

	case LM_V35:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=V35);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=V35<<3);
		break;

	case LM_449:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=RS449);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=RS449<<3);
		break;

	case LM_V28:
		if (escc->index == CH_A)
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=RS232);
		else
			api->write_reg_char ((void *)escc->base_address + MISC_CONTROL, reg|=RS232<<3);
		break;
	}


	// reset the channel
	if (escc->index == CH_A)
		escc->WriteReg.r9.wr9.bf.device_reset = 0x02;
	else
		escc->WriteReg.r9.wr9.bf.device_reset = 0x01;
	api->write_reg_char(escc->reg_address+REG9,escc->WriteReg.r9.wr9.reg);


	//zero the SCC register copy
	memset ((void *)&escc->WriteReg, 0, sizeof(_scc));

	//**	First wr4 (SCC manual says this MUST be first !!)
	// reset the register
	escc->WriteReg.r4.wr4.reg = 0;


	if(set->smode.rx && set->smode.rx != SMODE_ISO)
	{
		//sync
		escc->WriteReg.r4.wr4.bf.mode = SYNC_MODE_ENAB;
		switch(set->smode.rx)
		{
			case SMODE_EXT_SYNC:
				escc->WriteReg.r4.wr4.bf.sync_mode = EXT_SYNC_MODE;
				break;
			case SMODE_MONO:
				escc->WriteReg.r4.wr4.bf.sync_mode = SYNC_8_MODE;
				break;
			case SMODE_BISYNC:
				escc->WriteReg.r4.wr4.bf.sync_mode = SYNC_16_MODE;
				break;
			case SMODE_HDLC:
				escc->WriteReg.r4.wr4.bf.sync_mode = SDLC_MODE;
				break;
			default:
				return BAD_smode;
		}

		// clock mode	0 for synchronous mode
		escc->WriteReg.r4.wr4.bf.clock_mode = X1_CLK;
		//	parity info, none for sync comm.
		escc->WriteReg.r4.wr4.bf.parity = PARITY_NONE;

	}
	else
	{
		//async
        if(set->smode.rx == SMODE_ISO){
            escc->WriteReg.r4.wr4.bf.clock_mode = X1_CLK;
        }
        else {
            escc->WriteReg.r4.wr4.bf.clock_mode = X16_CLK;
        }
        
		switch(set->tx_frac_stop)
		{
			case 32:// 2 stop bits
				escc->WriteReg.r4.wr4.bf.mode = ASYNC_2_STOP;
				break;
			case 24: //1.5 stop bits
                if(set->smode.rx != SMODE_ISO){     // 1.5 stop bit is not allowed in Isosynchronus mode so we will drop to default in that case
                    escc->WriteReg.r4.wr4.bf.mode = ASYNC_15_STOP;
                    break;
                }
			default: // 1 stop bit
				escc->WriteReg.r4.wr4.bf.mode = ASYNC_1_STOP;
				break;
		}
		switch(set->parity.tx)
		{
			case PAR_EVEN:
				escc->WriteReg.r4.wr4.bf.parity = PARITY_EVEN;
				break;
			case PAR_ODD:
				escc->WriteReg.r4.wr4.bf.parity = PARITY_ODD;
				break;
			default:
				escc->WriteReg.r4.wr4.bf.parity = PARITY_NONE;
				break;
		}
	}
	//write to register WR4
	api->write_reg_char(escc->reg_address+REG4, escc->WriteReg.r4.wr4.reg);
	//printk("WR4:0x%x %x\n",escc->reg_address+REG4, escc->WriteReg.r4.wr4.reg);




	/*************************************************************************
	**	wr5
	**		TX data bits
	**		RTS state
	**		DTR state
	**/
	// reset the register
	escc->WriteReg.r5.wr5.reg = 0;

	if (set->crc.tx_type) {
		escc->WriteReg.r5.wr5.bf.tx_crc_enab = 1;
		if(set->smode.rx == SMODE_HDLC)
			set->crc.tx_type = CCITT;
		if (set->crc.tx_type == CCITT)
			escc->WriteReg.r5.wr5.bf.poly_sel = 0; // CCITT
		else
			escc->WriteReg.r5.wr5.bf.poly_sel = 1;	// CRC-16
	}


	// tx data size
	escc->WriteReg.r5.wr5.bf.tx_bits = set->dbits.tx - 5; // IUSC takes any number of bits; whereas ESCC starts from 5
	//write to register WR5
	api->write_reg_char(escc->reg_address+REG5,escc->WriteReg.r5.wr5.reg);
	//printk("WR5:0x%x %x\n",escc->reg_address+REG5, escc->WriteReg.r5.wr5.reg);

	/*************************************************************************
	**	wr6, wr7, wr10
	**	(use in SYNC modes only)
	**/

	/**
	**	wr6
	**/
	escc->WriteReg.r6.wr6.sync_sdlc_addr = set->sync_addr.rx0;
	api->write_reg_char(escc->reg_address+REG6,escc->WriteReg.r6.wr6.sync_sdlc_addr);
	//printk("WR6:0x%x %x\n",escc->reg_address+REG6, escc->WriteReg.r6.wr6.sync_sdlc_addr);

	/**
	**	wr7
	**/
	escc->WriteReg.r15.wr15.bf.select_wr7p = 0;		//enable wr7'
	api->write_reg_char (escc->reg_address+REG15, escc->WriteReg.r15.wr15.reg);
	if(set->smode.rx == SMODE_HDLC)
		escc->WriteReg.r7.wr7.sync_sdlc_flag = 0x7e;
	else
		escc->WriteReg.r7.wr7.sync_sdlc_flag = set->sync_addr.tx0;
	api->write_reg_char(escc->reg_address+REG7,escc->WriteReg.r7.wr7.sync_sdlc_flag);
	//printk("WR7:0x%x %x\n",escc->reg_address+REG7, escc->WriteReg.r7.wr7.sync_sdlc_flag);

	// Now WR3
	// reset the register
	escc->WriteReg.r3.wr3.reg = 0;
	// Rx data size 5, 6, 7, 8 bits
	escc->WriteReg.r3.wr3.bf.rx_bits = set->dbits.rx - 5; // IUSC takes any number of bits; whereas ESCC starts from 5


	if (set->async_settings.hflow == LSET_HFLOW_DCD_RX_CTS_TX)
		escc->WriteReg.r3.wr3.bf.auto_enab = 1;

	if(set->smode.rx == SMODE_HDLC) {		
		if (set->hdlc.rx_addr_control)
			escc->WriteReg.r3.wr3.bf.sdlc_search_mode = 1;
	}

	if(set->smode.rx != SMODE_ASYNC) {
		if (set->crc.rx_type)
			escc->WriteReg.r3.wr3.bf.rx_crc_enab = 1;
		else
			escc->WriteReg.r3.wr3.bf.rx_crc_enab = 0;

		if(set->sync_addr.strip_sync)
			escc->WriteReg.r3.wr3.bf.sync_load_inh = 1;
	}

	api->write_reg_char(escc->reg_address+REG3,escc->WriteReg.r3.wr3.reg);
	//printk("WR3:0x%x %x\n",escc->reg_address+REG3, escc->WriteReg.r3.wr3.reg);


	/**
	**	wr10
	**/
	// reset the register
	escc->WriteReg.r10.wr10.reg = 0;
	if(set->smode.rx != SMODE_ASYNC) {
	// 6 or 12 bit sync char enable
		if(set->sync_addr.rx_len == 6 || set->sync_addr.rx_len == 12)
			escc->WriteReg.r10.wr10.bf.six8_sync_sel = 1;

		if(set->smode.rx == SMODE_HDLC)
		{
			if(set->tx_pre_idle.tx_idle == IDLE_MARK)
			{
				escc->WriteReg.r10.wr10.bf.mark_flag_idle = 1;
			}
			else
			{
				escc->WriteReg.r10.wr10.bf.mark_flag_idle = 0;
			}
		}
		
		if (set->hdlc.tx_underrun)
			escc->WriteReg.r10.wr10.bf.abort_flag_on_underrun = 1;

		escc->WriteReg.r10.wr10.bf.crc_preset = set->crc.tx_start;
        
        switch(set->txclk.enc_dec)
        {
			case BIPH_LEVEL:
			case NRZ:
				escc->WriteReg.r10.wr10.bf.data_encoding = ENC_NRZ;
				break;
			case NRZ_INV:
				escc->WriteReg.r10.wr10.bf.data_encoding = ENC_NRZI;
				break;
			case BIPH_MARK:
				escc->WriteReg.r10.wr10.bf.data_encoding = ENC_FM1;
				break;
			case BIPH_SPACE:
				escc->WriteReg.r10.wr10.bf.data_encoding = ENC_FM0;
				break;
			default:
				return BAD_enc_dec;
        }

	}
    
	api->write_reg_char(escc->reg_address+REG10,escc->WriteReg.r10.wr10.reg);
	//printk("WR10:0x%x %x\n",escc->reg_address+REG10, escc->WriteReg.r10.wr10.reg);

	/**
	*************************************************************************
	**	wr11
	**/
	escc->WriteReg.r11.wr11.reg = 0;

	switch(set->txclk.clk_tree.A)
	{
		case CLK_RXC:
			escc->WriteReg.r11.wr11.bf.tx_clock = 0;
			break;
		case CLK_TXC:
			escc->WriteReg.r11.wr11.bf.tx_clock = 1;
			break;
		case CLK_BRG0:
			escc->WriteReg.r11.wr11.bf.tx_clock = 2;
			break;
		case CLK_DPLL:
			escc->WriteReg.r11.wr11.bf.tx_clock = 3;
			break;
		default:
			return TX_ERROR|BAD_clk_treeA;
	}

	switch(set->rxclk.clk_tree.A)
	{
		case CLK_RXC:
			escc->WriteReg.r11.wr11.bf.rx_clock = 0;
			break;
		case CLK_TXC:
			escc->WriteReg.r11.wr11.bf.rx_clock = 1;
			break;
		case CLK_BRG0:
			escc->WriteReg.r11.wr11.bf.rx_clock = 2;
			break;
		case CLK_DPLL:
			escc->WriteReg.r11.wr11.bf.rx_clock = 3;
			break;
		default:
			return RX_ERROR|BAD_clk_treeA;
	}


	switch(set->txclk.clk_pin)
	{
		case XC_INP:
			break;
		case RXC_RXCLK_ESCC:
			escc->WriteReg.r11.wr11.bf.trxc_source = 0;
			escc->WriteReg.r11.wr11.bf.trxc_io = 1;
			break;
		case TXC_TXCLK:
			escc->WriteReg.r11.wr11.bf.trxc_source = 1;
			escc->WriteReg.r11.wr11.bf.trxc_io = 1;
			break;
		case XC_BRG0:
			escc->WriteReg.r11.wr11.bf.trxc_source = 2;
			escc->WriteReg.r11.wr11.bf.trxc_io = 1;
			break;
		case XC_DPLL:
			escc->WriteReg.r11.wr11.bf.trxc_source = 3;
			escc->WriteReg.r11.wr11.bf.trxc_io = 1;
			break;
		default:
			return RX_ERROR|BAD_clk_pin;
	}

	escc->WriteReg.r11.wr11.bf.rtxc_xtal = 0; // comsync does not use this xtal


	api->write_reg_char(escc->reg_address+REG11,escc->WriteReg.r11.wr11.reg);
	//printk("WR11:0x%x %x\n",escc->reg_address+REG11, escc->WriteReg.r11.wr11.reg);
	/*************************************************************************
	**	wr12,13 (wr14)
	**		Bit Rate setup
	**	(first disable the BRG)
	**/
	escc->WriteReg.r14.wr14.bf.brg_enab = 0;
	api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
	//printk("WR14:0x%x %x\n",escc->reg_address+REG14, escc->WriteReg.r14.wr14.reg);

	/**
	**	X1 clock for Sync mode
	**/

	if (set->txclk.enc_dec && set->txclk.clk_tree.A == CLK_DPLL)
	{
		if ((set->txclk.enc_dec == BIPH_LEVEL) ||
			(set->txclk.enc_dec == BIPH_MARK) ||
			(set->txclk.enc_dec == BIPH_SPACE))
			escc->current_baud = set->txclk.bps * 16;
		else
			escc->current_baud = set->txclk.bps * 32;
       
	}
	else
        escc->current_baud = set->txclk.bps;

	if (!escc->current_baud)
		return BRG_computation;

	if(set->smode.rx)
		konst = 2;
	else
		konst = 32; //for async
	divisor = ((set->txclk.ref_freq / konst) / escc->current_baud) - 2;
	escc->WriteReg.r12.wr12.tc_low = (byte)(divisor & 0xff);
	escc->WriteReg.r13.wr13.tc_high = (byte)((divisor>>8) & 0xff);
	api->write_reg_char(escc->reg_address+REG12,escc->WriteReg.r12.wr12.tc_low);
	//printk("WR12:0x%x %x\n",escc->reg_address+REG12, escc->WriteReg.r12.wr12.tc_low);
	api->write_reg_char(escc->reg_address+REG13,escc->WriteReg.r13.wr13.tc_high);
	//printk("WR13:0x%x %x\n",escc->reg_address+REG13, escc->WriteReg.r13.wr13.tc_high);

	/**
	*************************************************************************
	**	wr14
	**		Enable baud rate generator and clock source=PCLK pin
	**		establish the DTR/REQ pin to be DTR
	**/

	if(set->lloop == 1)
		escc->WriteReg.r14.wr14.bf.local_loopback = 1;
	escc->WriteReg.r14.wr14.bf.brg_enab = 1; //enable BRG
	escc->WriteReg.r14.wr14.bf.brg_source = 1;//PCLK
	switch(set->txclk.clk_tree.C)
	{
		case BRG_INT_REF:
			escc->WriteReg.r14.wr14.bf.brg_source = 1;
			break;
		case BRG_RXC:
			escc->WriteReg.r14.wr14.bf.brg_source = 0;
			break;
		default:
			return TX_ERROR|BAD_clk_treeC;
	}

	api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
	//printk("WR14:0x%x %x\n",escc->reg_address+REG14, escc->WriteReg.r14.wr14.reg);

	if (set->txclk.enc_dec)
	{

		escc->WriteReg.r14.wr14.bf.dpll_cmd = DISABLE_DPLL;
		api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);


		switch(set->rxclk.clk_tree.B)
		{
			case DPLL_BRG0:
				escc->WriteReg.r14.wr14.bf.dpll_cmd = SRC_BRG;
				break;
			case DPLL_RXC:
				escc->WriteReg.r14.wr14.bf.dpll_cmd = SRC_RTXC;
				break;
			default:
				return TX_ERROR|BAD_enc_dec;
		}
		api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);


		if ((set->txclk.enc_dec == BIPH_LEVEL) ||
			(set->txclk.enc_dec == BIPH_MARK) ||
			(set->txclk.enc_dec == BIPH_SPACE))
			escc->WriteReg.r14.wr14.bf.dpll_cmd = FM_MODE; // see manual 3.3
		else
			escc->WriteReg.r14.wr14.bf.dpll_cmd = NRZI_MODE;
		api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
		//printk("WR14:0x%x %x\n",escc->reg_address+REG14, escc->WriteReg.r14.wr14.reg);


		escc->WriteReg.r14.wr14.bf.dpll_cmd = ENTER_SEARCH_MODE;
		api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
		//printk("WR14:0x%x %x\n",escc->reg_address+REG14, escc->WriteReg.r14.wr14.reg);
	}

	/*************************************************************************
	**	wr7p, wr1, wr15, wr2, wr9
	**		Interrupt(s) setup:
	**		Note: wr9 is common to both SCC channels, but can be
	**			addressed from either channel
	**/
	// Now wr2
	escc->WriteReg.r2.wr2.int_vector = 0;		//only one ESCC (2 channels) for now
	api->write_reg_char (escc->reg_address+REG2, escc->WriteReg.r2.wr2.int_vector);
	//printk("WR2:0x%x %x\n",escc->reg_address+REG2, escc->WriteReg.r2.wr2.int_vector);

	// First wr7' (wr7 prime)
	escc->WriteReg.r15.wr15.bf.select_wr7p = 1;		//enable wr7 Prime
	api->write_reg_char (escc->reg_address+REG15, escc->WriteReg.r15.wr15.reg);

	if (set->smode.tx == SMODE_HDLC)
		escc->WriteReg.r7p.bf.auto_tx_sdlc_flag = 1;
	else
		escc->WriteReg.r7p.bf.auto_tx_sdlc_flag = 0;

	if (set->async_settings.hflow == LSET_HFLOW_RTS_TOGGLE)
		escc->WriteReg.r7p.bf.auto_rts = 1;


	
	if (escc->use_dma_write || escc->flag_comsync_III)
		escc->WriteReg.r7p.bf.dtr_req_timing = 1; //DMA DTR

	if (set->crc.tx_type)
		escc->WriteReg.r7p.bf.auto_eom_reset = 1;

		
	
	escc->WriteReg.r7p.bf.tx_fifo_level = 1;		// TX interrupt when TX fifo empty
	
	if (escc->use_dma_read)
		escc->WriteReg.r7p.bf.rx_fifo_level = 0;	// RX interrupt when RX fifo >= 1 byte
	else
		escc->WriteReg.r7p.bf.rx_fifo_level = 1;

	escc->WriteReg.r7p.bf.ext_read_enab = 1;		// Extended Read enabled
	api->write_reg_char (escc->reg_address+REG7, escc->WriteReg.r7p.reg);
	//printk("WR7p:0x%x %x\n",escc->reg_address+REG7, escc->WriteReg.r7p.reg);

	escc->WriteReg.r15.wr15.bf.select_wr7p = 0;		//disable wr7'
	api->write_reg_char (escc->reg_address+REG15, escc->WriteReg.r15.wr15.reg);


	// Now wr15
	//escc->WriteReg.r15.wr15.bf.dcd_int_enab = 1;
	//escc->WriteReg.r15.wr15.bf.cts_int_enab = 1;
	if(set->smode.rx == SMODE_ASYNC)
			escc->WriteReg.r15.wr15.bf.break_int_enab = 1;
	if(set->smode.rx == SMODE_HDLC)
		escc->WriteReg.r15.wr15.bf.stat_fifo_enab = 1;
	if(set->smode.rx) {
		escc->WriteReg.r15.wr15.bf.sync_hunt_int_enab = 1;
		if(!escc->flag_comsync_III)
			escc->WriteReg.r15.wr15.bf.tx_underrun_int_enab = 1;
	}
	api->write_reg_char (escc->reg_address+REG15, escc->WriteReg.r15.wr15.reg);
	//printk("WR15:0x%x %x\n",escc->reg_address+REG15, escc->WriteReg.r15.wr15.reg);


	/**	wr0
	**		Reset TX CRC generator
	**		Reset TX Underrun/EOM latch
	**			(this primes the ESCC for CRC generation when
	**			the next TX underrun occurs).
	**/

	if(set->smode.rx != SMODE_ASYNC) {
		if (set->crc.tx_type)
		{
			if (!escc->WriteReg.r7p.bf.auto_eom_reset )
			{
				escc->WriteReg.r0.wr0.reg = 0;
				escc->WriteReg.r0.wr0.bf.crc_rst = RESET_TX_CRC_CHECKER;
				api->write_reg_char (escc->reg_address+REG0, escc->WriteReg.r0.wr0.reg);
			}


			escc->WriteReg.r0.wr0.reg = 0;
			escc->WriteReg.r0.wr0.bf.crc_rst = RESET_RX_CRC_CHECKER;
			api->write_reg_char (escc->reg_address+REG0, escc->WriteReg.r0.wr0.reg);


			escc->WriteReg.r0.wr0.reg = 0;
			escc->WriteReg.r0.wr0.bf.crc_rst = RESET_TX_UNDERRUN_OEM_LATCH;
			api->write_reg_char (escc->reg_address+REG0, escc->WriteReg.r0.wr0.reg);
		}
	}

	/**
	*************************************************************************
	**	wr3
	**		Enable Receiver
	**/
	if(set->smode.rx != SMODE_ASYNC)
	{
		escc->hunt_mode = 1;
		escc->WriteReg.r3.wr3.bf.enter_hunt = 1;
	}

	escc->WriteReg.r3.wr3.bf.rx_enab = 1;
	api->write_reg_char(escc->reg_address+REG3,escc->WriteReg.r3.wr3.reg);
	//printk("WR3:0x%x %x\n",escc->reg_address+REG3, escc->WriteReg.r3.wr3.reg);



	escc->WriteReg.r9.wr9.bf.soft_int_ack = 0;
	escc->WriteReg.r9.wr9.bf.device_reset = 0;
	escc->WriteReg.r9.wr9.bf.vector_incl_stat = 1;
	escc->WriteReg.r9.wr9.bf.no_vector_sel = 0;
	escc->WriteReg.r9.wr9.bf.status_high = 0;

	escc->WriteReg.r9.wr9.bf.mie = 1;				// this is the master Interrupt enable
	api->write_reg_char (escc->reg_address+REG9, escc->WriteReg.r9.wr9.reg);
	//printk("WR9:0x%x %x\n",escc->reg_address+REG9, escc->WriteReg.r9.wr9.reg);

	/**
	*************************************************************************
	**	wr0
	**		Reset EXT_STATUS interrupts (2 times)
	**		this prepares the SCC to generate EXT_STATUS interrupts
	**/

	escc->WriteReg.r0.wr0.reg = 0;
	escc->WriteReg.r0.wr0.bf.cmd = RESET_EXT_STAT_INTERRUPTS;
	api->write_reg_char (escc->reg_address+REG0, escc->WriteReg.r0.wr0.reg);
	api->write_reg_char (escc->reg_address+REG0, escc->WriteReg.r0.wr0.reg);

	// clear TC INT
	api->write_reg_char((void *)escc->base_address+CS104_TC_Secur,0xff);

	escc->ReadReg.r0.rr0.reg = 0;

	// Now wr1
	escc->WriteReg.r1.wr1.reg = 0;

	escc->WriteReg.r1.wr1.bf.ext_stat_mie = 1;		//EXT status interrupts

	if (escc->use_dma_write || escc->flag_comsync_III)
		escc->WriteReg.r1.wr1.bf.tx_int_enab = 0;//DMA
	else
		escc->WriteReg.r1.wr1.bf.tx_int_enab = 1;		//TX interrupt enable
	
	escc->WriteReg.r1.wr1.bf.parity_special = 0;		//No parity error interrupts
	// RX interrupt
	if (escc->flag_comsync_III)
	{
		escc->WriteReg.r1.wr1.bf.rx_int_mode = RX_INT_ON_SPEC_CON_ONLY;
	}
	else if (escc->use_dma_read && !escc->flag_comsync_III)
		escc->WriteReg.r1.wr1.bf.rx_int_mode = RX_INT_ON_FIRST_CH_OR_SPEC_CON;	
	else
		escc->WriteReg.r1.wr1.bf.rx_int_mode = INT_ON_ALL_RX_CH_OR_SPEC_CON;
		

	api->write_reg_char (escc->reg_address+REG1, escc->WriteReg.r1.wr1.reg);
	//printk("WR1:0x%x %x\n",escc->reg_address+REG1, escc->WriteReg.r1.wr1.reg);


	return 0;
}



// This will reset a channel
void reset_escc_ch(struct sync_port_drv_struct *sync_port)
{
	struct _api_context *api = &sync_port->api;

	sync_port->escc.WriteReg.r14.wr14.bf.brg_enab = 0;
	api->write_reg_char(sync_port->escc.reg_address+REG14,sync_port->escc.WriteReg.r14.wr14.reg);
	
	if (sync_port->escc.index == CH_A)
		sync_port->escc.WriteReg.r9.wr9.bf.device_reset = 0x02;
	else
		sync_port->escc.WriteReg.r9.wr9.bf.device_reset = 0x01;	
	api->write_reg_char(sync_port->escc.reg_address+REG9,sync_port->escc.WriteReg.r9.wr9.reg);
}


void
start_dma_dtr_req(struct escc_struct *escc, struct _api_context *api)
/**************************************************************************

	This routine will kick start DMA write for ESCC
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
		Return:
			None
**************************************************************************/

{
	escc->WriteReg.r14.wr14.bf.dtr_req = 1;
	api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
}

void
stop_dma_dtr_req(struct escc_struct *escc, struct _api_context *api)
/**************************************************************************

	This routine will stop DMA write for ESCC
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
		Return:
			None
**************************************************************************/

{
	escc->WriteReg.r14.wr14.bf.dtr_req = 0;
	api->write_reg_char(escc->reg_address+REG14,escc->WriteReg.r14.wr14.reg);
}

void
start_dma_wreq_read(struct escc_struct *escc, struct _api_context *api)
/**************************************************************************

	This routine will kick start DMA read for ESCC
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
		Return:
			None

**************************************************************************/

{
	escc->WriteReg.r1.wr1.bf.wait_dma = 1;
	//api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
	escc->WriteReg.r1.wr1.bf.wait_req = 1;
	api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
	escc->WriteReg.r1.wr1.bf.wait_dma_enab = 1;
	api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
}

void
stop_dma_wreq_read(struct escc_struct *escc, struct _api_context *api)
/**************************************************************************

	This routine will kick start DMA read for ESCC
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
		Return:
			None

**************************************************************************/

{
	escc->WriteReg.r1.wr1.bf.wait_dma = 0;
	api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
	escc->WriteReg.r1.wr1.bf.wait_req = 0;
	api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
	escc->WriteReg.r1.wr1.bf.wait_dma_enab = 0;
	api->write_reg_char(escc->reg_address+REG1,escc->WriteReg.r1.wr1.reg);
}

void escc_set_rts(struct escc_struct *escc, struct _api_context *api, char set_reset)
/**************************************************************************

	This routine will set/clear RTS
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
			'set_reset'		set/reset value
		Return:
			None

**************************************************************************/

{
	escc->WriteReg.r5.wr5.bf.rts = set_reset;
	api->write_reg_char(escc->reg_address+REG5,escc->WriteReg.r5.wr5.reg);
}

void escc_set_dtr(struct escc_struct *escc, struct _api_context *api, char set_reset)
/**************************************************************************

	This routine will set/clear DTR
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
			'set_reset'		set/reset value
		Return:
			None

**************************************************************************/

{
	escc->WriteReg.r5.wr5.bf.dtr = set_reset;
	api->write_reg_char(escc->reg_address+REG5,escc->WriteReg.r5.wr5.reg);
}

void escc_send_break(struct escc_struct *escc, struct _api_context *api, char set_reset)
/**************************************************************************

	This routine will set/clear break
		Args:
			'escc'			a pointer to escc_struct structure
			'api'			a pointer to _api_context structure
			'set_reset'		set/reset value
		Return:
			None

**************************************************************************/

{
	escc->WriteReg.r5.wr5.bf.tx_break = set_reset;
	api->write_reg_char(escc->reg_address+REG5,escc->WriteReg.r5.wr5.reg);
}
