/*******************************************************************************
**	File:			iusc.c
**	Author:			Rod Dore, Hasan Jamal
**
**	Description:
**		Functions and data for setup of IUSC and related hardware on the BHN-Sync
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

// Code body
#ifdef LINUX
	#include <asm/io.h>
#endif

#include "precomp.h"


#define FOR_RECEIVER		1
#define FOR_TRANSMITTER		2




	//**************************************************************************
	//	Reset the IUSC device and establish it's basic operation for
	//	this bus
	//		Args:
	//			'port'	a value from 0 to N-1
	//		Return:
	//			nothing
	//
#ifdef QNX4
	void iusc_reset (int port, void __far *v_iusc, void __far *v_pld, void *context)
#else
	void iusc_reset (int port, void *v_iusc, void *v_pld, void *context)
#endif

	{
		int i;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif

		// Disable interrupts to prevent the following sequence from being disturbed
//		board_irq_disable();

		// Reset the chip
		//	This reset operation also places the Line Interface into a Tri-State condition
		api->write_reg_word((void *)&pld->pld_cntrl, (api->read_reg_word((void *)&pld->pld_cntrl) & ~SOFT_RESET) | PN_RST(port));
		i = 10; while (i--);
		api->write_reg_word((void *)&pld->pld_cntrl, api->read_reg_word((void *)&pld->pld_cntrl) & ~SOFT_RESET);
		i = 20; while (i--);

		// The next write operation sets the BCR of the IUSC
		//		The DCAR address is used to force the S//D pin of the IUSC low
		//		during the write operation, which established the mode of operation
		//		of the /WAIT//READY pin of the IUSC.
		//&iusc->DCAR.WORD = 0x000D;	// this is NOT a write to the DCAR register
		api->write_reg_word((void *)&iusc->DCAR.WORD, 0x000D);


		// Setup registers to enable operation on a big endian bus
		api->write_reg_word((void *)&iusc->CCAR.WORD, D7_0_FIRST);
	}

	//**************************************************************************
	//	Set an LIFC register in the PLD
	//		This register goes busy (for about 16uS) every time it is written to
	//		Args:
	//			'port'			a value from 0 to N-1
	//			'pld'			pointer to pld structure
	//			'new_value'		value
	//		Return:
	//			The value previously in the LIFC register
	//
#ifdef QNX4
	unsigned short write_lifc (int port, void __far *v_pld, void *context, unsigned short new_value)
#else
	unsigned short write_lifc (int port, void *v_pld, void *context, unsigned short new_value)
#endif
	{
		unsigned short old_value;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _main_pld __far *pld;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _main_pld *pld;
		pld = (_main_pld *)v_pld;
#endif


		// save the current value, write the new value
		old_value = api->read_reg_word((void *)&pld->lifc[port].cntrl);
		api->write_reg_word((void *)&pld->lifc[port].cntrl, new_value);

		return (old_value);
	}

	//**************************************************************************
	//	Read an LIFC register
	//	Args:
	//		port		Port number (0 to N-1)
	//
#ifdef QNX4
	unsigned short read_lifc (int port, void __far *v_pld, void *context)
#else
	unsigned short read_lifc (int port, void *v_pld, void *context)
#endif
	{
		struct _api_context *api = (struct _api_context *)context;

#ifdef QNX4
		volatile _main_pld __far *pld;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _main_pld *pld;
		pld = (_main_pld *)v_pld;
#endif


		// save the current value, write the new value
		return(api->read_reg_word((void *)&pld->lifc[port].cntrl));
	}

	//**************************************************************************
	//	Enable or Disable PLD (IUSC and RTC) Interrupts
	//	Args:
	//		enab		=0,		disable the MIE bit
	//					!=0,	enable the MIE bit
	//
#ifdef	QNX4
	void set_pld_int (int enab, void __far *v_pld, void *context)
#else
	void set_pld_int (int enab, void *v_pld, void *context)
#endif
	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _main_pld __far *pld;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _main_pld *pld;
		pld = (_main_pld *)v_pld;
#endif

		if (enab) api->write_reg_word((void *)&pld->pld_cntrl, api->read_reg_word((void *)&pld->pld_cntrl) |MASTER_IE);
		else api->write_reg_word((void *)&pld->pld_cntrl, api->read_reg_word((void *)&pld->pld_cntrl) & ~MASTER_IE);
	}

	//**************************************************************************
	//	Enable or Disable the Master Serial Interrupt bit in the IUSC
	//	Args:
	//		port		Port number (0 to N-1)
	//		enab		=0,		disable the MIE bit
	//					!=0,	enable the MIE bit
	//
#ifdef QNX4
	void iusc_set_serial_mie (void __far *v_iusc, void *context, int enab)
#else
	void iusc_set_serial_mie (void *v_iusc, void *context, int enab)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif



		if (enab) api->write_reg_word((void *)&iusc->ICR.WORD, api->read_reg_word((void *)&iusc->ICR.WORD) | MIE);
		else api->write_reg_word((void *)&iusc->ICR.WORD, api->read_reg_word((void *)&iusc->ICR.WORD) & ~MIE);
	}

	//**************************************************************************
	//	Enable or disable the Master DMA Interrupt bit in the IUSC
	//	Args:
	//		port		Port number (0 to N-1)
	//		enab		=0,		disable the MIE bit
	//					!=0,	enable the MIE bit
	//
#ifdef QNX4
	void iusc_set_dma_mie (void __far *v_iusc, void *context, int enab)
#else
	void iusc_set_dma_mie (void *v_iusc, void *context, int enab)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		if (enab) api->write_reg_word((void *)&iusc->DICR.WORD, api->read_reg_word((void *)&iusc->DICR.WORD) | DMA_MIE);
		else api->write_reg_word((void *)&iusc->DICR.WORD, api->read_reg_word((void *)&iusc->DICR.WORD) & ~DMA_MIE);
	}

	//**************************************************************************
	//	Return register settings for DPLL divisor settings
	//
	unsigned short dpll_div_set (unsigned char setting)
	{
		switch (setting) {
			case 8:
				return (DPLL_DIV8);
			case 16:
				return (DPLL_DIV16);
			case 32:
				return (DPLL_DIV32);
		}
		return (0);
	}

	//**************************************************************************
	//	Return register settings for CTR divisor settings
	//
	unsigned short ctr_div_set (unsigned char ctr, unsigned char setting)
	{
		if (ctr == 0) {
			switch (setting) {
				case 4:
					return (CTR0_DIV4);
				case 8:
					return (CTR0_DIV8);
				case 16:
					return (CTR0_DIV16);
				case 32:
					return (CTR0_DIV32);
			}
		}
		else {
			//ctr1
			switch (setting) {
				case 4:
					return (CTR1_DIV4);
				case 8:
					return (DPLL_DIV8);
				case 16:
					return (DPLL_DIV16);
				case 32:
					return (DPLL_DIV32);
			}
		}
		return (0);
	}

	//**************************************************************************
	//	Return register settings for CTR source settings
	//
	unsigned short ctr_src_set (unsigned char ctr, unsigned char setting)
	{
		if (ctr == 0) {
			switch (setting) {
				case CTR_DISABLE:
					return (CTR0_DISABLE);
				case CTR_REF:
					return (CTR0_PORT0);
				case CTR_RXC:
					return (CTR0_RXC);
				case CTR_TXC:
					return (CTR0_TXC);
			}
		}
		else {
			//ctr1
			switch (setting) {
				case CTR_DISABLE:
					return (CTR1_DISABLE);
				case CTR_REF:
					return (CTR1_PORT1);
				case CTR_RXC:
					return (CTR1_RXC);
				case CTR_TXC:
					return (CTR1_TXC);
			}
		}
		return (0);
	}



	//**************************************************************************
	//	Setup a CTR (divisor and source)
	//		Args:
	//			iusc		pointer to IUSC chip
	//			ctr			counter nummber (0 or 1)
	//
#ifdef QNX4
	int setup_ctr(void __far *v_iusc, void *context, unsigned char ctr, unsigned char divisor, unsigned char src_setting, unsigned char bypass_request, char validate, struct _resource *resource)
#else
	int setup_ctr(void *v_iusc, void *context, unsigned char ctr, unsigned char divisor, unsigned char src_setting, unsigned char bypass_request, char validate, struct _resource *resource)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		// Is bypass being requested?
		if (bypass_request || (divisor == 1)) {
			// Is the bypass already set?
			if (resource->ctrs_bypassed)	return (SUCCESS);
			// check to see if either counter is being used already (ie: a divisor value has been setup)
			if ((resource->used_ctr_div[0] != 0) || (resource->used_ctr_div[1] != 0)) {
				return (GENERAL_ERROR + INVALID_ERROR + CTR_overload);
			}
			if (!validate) api->write_reg_word((void *)&iusc->CCSR.WORD, api->read_reg_word((void *)&iusc->CCSR.WORD) | CTR01_BYPASS);
			resource->ctrs_bypassed = 1;
			return (SUCCESS);
		}
		//	Determine if the CNTR's have already been bypassed, if so, the setup of the counter is useless
		else if (resource->ctrs_bypassed) {
			return (GENERAL_ERROR + INVALID_ERROR + CTR_overload);
		}

		// Check to see if the CTR is already being used with some other divisor value
		if ((resource->used_ctr_div[ctr] != 0 ) && (resource->used_ctr_div[ctr] != divisor)) {
			return (GENERAL_ERROR + INVALID_ERROR + CTR_overload);
		}
		//	mark the resource as used, with a certain divisor setting
		resource->used_ctr_div[ctr] = divisor;

		// Check to see if the CTR is already being used with some other clock source?
		if ((resource->used_ctr_src[ctr] != 0) && (resource->used_ctr_src[ctr] != (src_setting | 0x40))) {
			return (GENERAL_ERROR + INVALID_ERROR + CTR_overload);
		}
		resource->used_ctr_src[ctr] = src_setting | 0x40;	// set bit 6 to ensure a non-zero value

		//	Set counter divisor & source
		switch (ctr) {
			case 0:
				if (!validate) {
					api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~CTR0_DIV) | ctr_div_set(0, divisor));
					api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~CTR0_SRC) | ctr_src_set (0, src_setting));
				}
				break;
			case 1:
				if (!validate) {
					api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~DPLL_DIV) | ctr_div_set(1, divisor));
					api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~CTR1_SRC) | ctr_src_set (1, src_setting));
				}
				break;
			default:
				return (GENERAL_ERROR + CODE_ERROR);
		}

		return (SUCCESS);
	}



	//**************************************************************************
	//	Storage for the values calculated by the following function
	//
	struct brg_values {
		unsigned long divisor;
		unsigned long prescale;
	} _brgv;

	//**************************************************************************
	//	Calculation of divisors for the Baud Rate generator and the Prescaler counter
	//		Args:
	//			'ref_freq'			frequency of reference clock
	//			'bps'				bit rate (integer portion)
	//			'bps_frac'			bit rate (fractional portion, in 1/10's), use value of 0 if not required
	//			'corr_fact'			other divisors (DPLL or ASYNC mode settings), use a value of 1 if not required
	//			'target_ppm'		desired target bit rate error (in PPM)
	//			'fixed_prescale'	the prescale value is fixed (this occurs when the DPLL is used, or when a previous resource setup
	//								caused the CTR bypass to occur), use a value of 0 if not required
	//		Return:
	//			Pointer to a structure containing the BRG divisor and prescale values computed from the input parameters
	//			0		if there is an error
	//
	//		The goal is to keep the prescaler "in" the divisor chain until the desired bit rate is too high to
	//		allow the use of the prescaler. Why?... For maximum versatility of the divisor circuits, its
	//		NOT desirable to enable the CTR0/1 by-pass bit, because this bit causes both CTR's to be by-passed
	//		simultaneously.
	//
	struct brg_values *calc_brg (
		unsigned long ref_freq,		unsigned long bps,
		unsigned long bps_frac,		unsigned long corr_fact,
		unsigned long target_ppm,	unsigned char fixed_prescale)
	{
		unsigned long div_x10, prescale, bps_x10, target_clk, actual_clk=0, error, ppm;

		// combine the bps setting and the fractional bps setting
		//	to create a number that is 10X the desired bit rate
		bps_x10 = (bps * 10L) + bps_frac;

		// check for some zero values
		if ((bps_x10 == 0) || (ref_freq == 0) || (corr_fact == 0)) {
			// the computation can't be performed (divide by zero errors will occur)
			return (0);
		}

		//check for absolute maximum bit rate (in any clocking mode)
		if (bps_x10 > (ref_freq * 10L)) {
			// bit rate cannot be achieved
			return (0);
		}

		target_clk = corr_fact * bps_x10;			// bring in the correction factor
		if (fixed_prescale == 0) prescale = 64;		// initial prescale value (32x2)
		else prescale = fixed_prescale;
		error = 0;

		do {
			do {
				if (fixed_prescale == 0) {
					prescale /= 2;		// next (or first) prescale value is half the previous
					if (prescale < 4) {
						// counter must be bypassed
						prescale = 1;
					}
				}
				// calculate a divisor that is 10X the actual divisor,
				//	this allows a rounding function to be performed after
				div_x10 = (((ref_freq / prescale) * 100L) / target_clk);

				// round up or down (use 'error' as a temporary variable)
				error = div_x10;
				div_x10 /= 10;
				if ((error % 10) >= 5) div_x10 += 1;
				div_x10 *= 10;

				// test for a very low divisor value (go to the next prescale value immediately)
				if (div_x10 == 0) {
					if (prescale == 1) {
						// can't achieve this bit rate & correction factor combination (it's too high)
						return (0);
					}
					continue;
				}

				//check for minimum bit rate
				if (((fixed_prescale != 0) || (prescale == 32)) && (div_x10 > 655360)) {
					// bit rate cannot be achieved (it's too low)
					return (0);
				}

				// Compute the resulting frequency of the internal clock (x10)
				actual_clk = ((ref_freq / prescale) * 100L) / div_x10;

				// look for magical combinations which produce
				//	zero error, with high prescale & low divisor settings
				if (actual_clk == target_clk) goto calc_done;		// test for zero error

			// at high bit rates the divisor will be small,
			//	therefore the prescale must be made smaller
			} while ((fixed_prescale == 0) && (prescale > 1) && (div_x10 < 100));		// 100 is actually 10.0

			//	compute absolute error (in ppm), ('actual_clk' is calculated above)
			//	this computation can "blow up" (exceed 32 bit numbers) quite easily (especially when the target and actual clocks are high)
//			if (actual_clk > target_clk)	error = (((actual_clk - target_clk) * 10000) / target_clk) * 100;
//			else							error = (((target_clk - actual_clk) * 10000) / target_clk) * 100;

			if (actual_clk > target_clk) error = actual_clk - target_clk;
			else error = target_clk - actual_clk;
			// convert to PPM
			ppm = 1000000;		// 10^6
			// make the 'error' value as large as possible before dividing it by the 'target_clk',
			//	this maximizes the accuracy of the calculation.
			while ((error <= 2147483647) && (ppm > 61)) {		// constant is ((2^32)/2) -1
				error <<= 1;	//error *= 2;
				ppm >>= 1;		//ppm /= 2;
			}
			error = (error / target_clk) * ppm;

		// at low bit rates the error produced by a large prescale value could be large,
		//	therefore the prescale must be made smaller
		} while ((fixed_prescale == 0) && (prescale > 1) && (error > target_ppm));

		// determine if the bit rate error is above the desired error
		if ((prescale == 1) && (error > target_ppm)) {
			return (0);
		}

		// assign the values to the return structure
		calc_done:
		_brgv.divisor = (div_x10 / 10) - 1;	// fix the divisor value (appropriate for the TC0/1R register)
		_brgv.prescale = prescale;

		return (&_brgv);
	}


	//**************************************************************************
	//	Setup a Baud rate generator and a prescale counter to achieve a target bit rate
	//		Args:
	//			'set'			a pointer to a settings structure
	//			'target'		indicates whether the setup is for the Receiver or Transmitter
	//			'brg'			selects the BRG to setup
	//		Return:
	//			0		success
	//			>0		failure
	//
#ifdef QNX4
	int setup_brg_ctr (BHN_PortSettings *set, void __far *v_iusc, void *context, unsigned char target, unsigned char brg, char validate, struct _resource *resource)
#else
	int setup_brg_ctr (BHN_PortSettings *set, void *v_iusc, void *context, unsigned char target, unsigned char brg, char validate, struct _resource *resource)
#endif

	{
		volatile unsigned short *tcr;	//	pointer to TCxR register //March 14, 2008 Karl added volatile
		_trclk *trclk;
		struct brg_values *brgv;
		unsigned char tree_C;
		int temp;
		unsigned long ref_freq;

		unsigned long corr_fact = 1;
		unsigned char fixed_prescale = 0;

		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif

		if (target == FOR_RECEIVER) {
			// point to the Receiver clocking control structure
			trclk = &set->rxclk;
			if (set->smode.rx == SMODE_ASYNC) corr_fact *= trclk->async_div;		//adjust the correction factor

			// Is the DPLL between the BRG and the Receiver
			if ((trclk->clk_tree.A & 0x7F) == CLK_DPLL) {
				switch (trclk->clk_tree.B) {
					case DPLL_BRG0:
					case DPLL_BRG1:
						corr_fact *= trclk->dpll_div;		//adjust the correction factor
						// Is the BRG using CTR-1?
						//	if so the prescale divisor is fixed to a setting that is the same as the DPLL rate.
						if ((trclk->clk_tree.C & ~BRG_INT_REF) == BRG_CTR1) {
							fixed_prescale = trclk->dpll_div;
						}
					default:
						break;
				}
			}
		}
		else if (target == FOR_TRANSMITTER) {
			// point to the Transmitter clocking control structure
			trclk = &set->txclk;
			if (set->smode.tx == SMODE_ASYNC) corr_fact *= trclk->async_div;		//adjust the correction factor
		}
		//	something is wrong with the 'target' parameter
		else
		{
			return (GENERAL_ERROR + CODE_ERROR);
		}

		// if the previous setup of receiver, transmitter or clock output pin, has set the counter bypass, then
		//	it must be carried along into subsequent setups (setting the fixed_prescale value to 1 will cause this effect)
		if (resource->ctrs_bypassed) fixed_prescale = 1;

		// override 'ref_freq' value when the clocking source for the BRG is CTR0 or Port-0
		if ((trclk->clk_tree.C & 0x7F) == BRG_CTR0) ref_freq = INT_REF_FREQ;
		else ref_freq = trclk->ref_freq;

		// compute BRG divisor and prescaler setting
		brgv = calc_brg (ref_freq, trclk->bps, trclk->bps_frac, corr_fact, trclk->bps_error, fixed_prescale);

		// check the result of the 'calc_brg' function
		if (brgv == 0) {
			// bit rate requested cannot be achieved
			switch (target) {
				case FOR_RECEIVER:
					return (RX_ERROR + INVALID_ERROR + BRG_computation);
				case FOR_TRANSMITTER:
					return (TX_ERROR + INVALID_ERROR + BRG_computation);
			}
		}

		if ((brg == CLK_BRG0) || (brg == XC_BRG0)) {
			// Check to see if the Receiver is using this BRG at a setting that is different from the Transmitter
			if ((target == FOR_TRANSMITTER) && (resource->used_brg_tc[0] != 0 ) && (resource->used_brg_tc[0] != brgv->divisor)) {
				// Receiver divisor and Transmitter divisor for this BRG don't match
				//	so the setting can't be achieved
				return (GENERAL_ERROR + INVALID_ERROR + BRG_overload);
			}
			tcr = &iusc->TC0R.WORD;
			resource->used_brg_tc[0] = brgv->divisor;
		}
		else if ((brg == CLK_BRG1) || (brg == XC_BRG1)) {
			if ((target == FOR_TRANSMITTER) && (resource->used_brg_tc[1] != 0 ) && (resource->used_brg_tc[1] != brgv->divisor)) {
				// Receiver divisor and Transmitter divisor for this BRG don't match
				//	so the setting can't be achieved
				return (GENERAL_ERROR + INVALID_ERROR + BRG_overload);
			}
			tcr = &iusc->TC1R.WORD;
			resource->used_brg_tc[1] = brgv->divisor;
		}
		//	the BRG selection make no sense
		else{
			 return (GENERAL_ERROR + CODE_ERROR);
		 }

		// set the divisor value into the appropriate BRG register
		if (!validate) api->write_reg_word((void*)tcr, brgv->divisor);

		//	setup the BRG clocking source
		tree_C = trclk->clk_tree.C;
		if ((brg == CLK_BRG0) || (brg == XC_BRG0)) {
			//	Check to see if the BRG-0 resource is being used with some other clocking source
			if ((resource->used_brg_src[0] != 0) && (resource->used_brg_src[0] != ((tree_C & 0x7f) | 0x40))) {
				return (GENERAL_ERROR + INVALID_ERROR + BRG_overload);
			}
			// set the BRG0 source and enable it
			if (!validate) {
				api->write_reg_word((void *)&iusc->CMCR.WORD, (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~BRG0_SRC) | BRG0_SRC_SET(tree_C));
				api->write_reg_word((void *)&iusc->HCR.WORD, api->read_reg_word((void *)&iusc->HCR.WORD) & ~BRG0_SINGLE);		//	Always run BRG's in continuous mode
				api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD)| BRG0_ENAB);
			}
			resource->used_brg_src[0] = (tree_C & 0x7F) | 0x40;	//bit 6 is added to make the value non-zero
		}
		else if ((brg == CLK_BRG1) || (brg == XC_BRG1)) {
			//	Check to see if the BRG-1 resource is being used with some other clocking source
			if ((resource->used_brg_src[1] != 0) && (resource->used_brg_src[1] != ((tree_C & 0x7f) | 0x40))) {
				return (GENERAL_ERROR + INVALID_ERROR + BRG_overload);
			}
			// set the BRG1 source and enable it
			if (!validate) {
				api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD)& ~BRG1_SRC) | BRG1_SRC_SET(tree_C));
				api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD) & ~BRG1_SINGLE);
				api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD)| BRG1_ENAB);
			}
			resource->used_brg_src[1] = (tree_C & 0x7F) | 0x40;
		}

		// setup the prescaler counter, which CTR?
		if ((tree_C & BRG_INT_REF) || (brgv->prescale == 1)) {
			//	request that the bypass be set
			if ((temp = setup_ctr((void*)iusc, api, 0, 0, 0, 1, validate,resource)) != SUCCESS) {
				return (temp);
			}
		}
		else {
			// CTR not by-passed
			switch (tree_C & 0x7F) {
				case BRG_CTR0:
				case BRG_CTR1:
					if ((temp = setup_ctr((void*)iusc, api, tree_C, brgv->prescale, trclk->clk_tree.D, 0, validate, resource)) != SUCCESS) {
						return (temp);
					}
					break;
				default:
					break;
			}
		}
		return (SUCCESS);
	}


	//**************************************************************************
	//	Setup TX and RX clocking
	//		Args:
	//			'set'			a pointer to a settings structure
	//			validate		1= validate the setup (without touching the hardware)
	//							0= normal setup
	//		Return:
	//			0	=	success
	//			>0	=	failure
	//
	//	This function will setup the clocking for both the Transmitter and Receiver
	//		based on settings indicated in the setup data structure.
	//	This is a fairly complicated process due to the considerable number of clocking
	//		choices available in the IUSC. This function ATTEMPTS to discover illegal
	//		or invalid setting combinations (returning a failure), but there are combinations
	//		that are will not operate properly which will not be detected by this function.
	//
#ifdef QNX4
	int setup_iusc_clk (BHN_PortSettings *set, void __far *v_iusc, void __far *v_pld, void *context, char validate)
#else
	int setup_iusc_clk (BHN_PortSettings *set, void *v_iusc, void *v_pld, void *context, char validate)
#endif

	{
		int temp;
		unsigned char tree_A;
		struct _api_context *api = (struct _api_context *)context;
		struct _resource resource;

#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif

		if (!validate) {
			//	Setup starting conditions
			//	Disable the Receiver and Transmitter clock sources
			api->write_reg_word((void *)&iusc->CMCR.WORD,  api->read_reg_word((void *)&iusc->CMCR.WORD) & ~(RXCLK_SRC | TXCLK_SRC));
			//	Stop BRG0/1 (must be stopped before any changes can be made)
			api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD) & ~(BRG0_ENAB | BRG1_ENAB));
			//	Disable DPLL
			api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD) &  ~(DPLL_MODE));
			//	Control CTR1's divisor from the DPLL divisor setting bits
			//	Note: a correction to the prescale setting for CTR1 must be applied to
			//		any calculation of the bit rate divisor settings.
			api->write_reg_word((void *)&iusc->HCR.WORD,  api->read_reg_word((void *)&iusc->HCR.WORD) | CTR1_DPLL_DIV);
			//	CTR0/1 bypass disabled
			api->write_reg_word((void *)&iusc->CCSR.WORD,  api->read_reg_word((void *)&iusc->CCSR.WORD) & ~CTR01_BYPASS);
			//	CTR0/1 disabled
			api->write_reg_word((void *)&iusc->CMCR.WORD,  api->read_reg_word((void *)&iusc->CMCR.WORD) & ~(CTR0_SRC | CTR1_SRC));
		}

		//	zero the "clocking resources used" storage area
		memset (&resource, 0, sizeof(resource));

		//------------------------------------------------------------
		// Receiver first
		//	Check for external/direct clocking sources
		tree_A = set->rxclk.clk_tree.A;

		// The code may loop back to this point under certain clocking situations
		setup_rx_clk:
		switch (tree_A) {
			case CLK_INT_REF:
			case CLK_EXT_REF:
				// there is only one bypass control bit which affects both CTR's
				if ((temp = setup_ctr((void*)iusc, api, 0,0,0,1, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				// fall through to next case

			case CLK_NONE:
				break;

			case CLK_RXC:
				// Determine if RxC is driven from an internal source
				//	Some of the possible sources produce a "circular" clock structure
				//	which would be non-functional.
				switch (set->rxclk.clk_pin) {
					case XC_INP:
						// The source for the RxC pin is an external signal
						break;
					case RXC_RXCLK:
					case XC_CHAR_CLK:
					case RXC_RXSYNC:
						// These cases are "circular" clocking, which yields a non-functional setup
						return (RX_ERROR + INVALID_ERROR + CIRCULAR_clocking);
					// These next 4 cases can be handled by pretending that the Receiver is being
					//	clocked by a source that is equivalent to the clock source for the RxC pin.
					case XC_BRG0:
						tree_A = CLK_BRG0;
						goto setup_rx_clk;
					case XC_BRG1:
						tree_A = CLK_BRG1;
						goto setup_rx_clk;
					case RXC_CTR0:
						tree_A = CLK_CTR0;
						goto setup_rx_clk;
					case XC_DPLL:
						tree_A = CLK_DPLL;
						goto setup_rx_clk;
					case RXC_PORT0:
						tree_A = CLK_INT_REF;
						goto setup_rx_clk;
					default:
						// bad setting
						return (RX_ERROR + PARAM_ERROR + BAD_clk_pin);
						break;
				}
				break;

			case CLK_TXC:
				// determine if TxC is driven from an internal source
				//	Some of the possible sources produce a "circular" clock structure
				//	which would be non-functional.
				switch (set->txclk.clk_pin) {
					case XC_INP:
						// The source for the TxC pin is an external signal
						break;
					case TXC_TXCLK:
					case XC_CHAR_CLK:
					case TXC_TXCOMP:
						// These combinations might be circular if the Transmitter is
						//	also clocked from the TxC pin
						if (set->txclk.clk_tree.A == CLK_TXC) {
							return (TX_ERROR + INVALID_ERROR + CIRCULAR_clocking);
						}
						break;
					// These next 4 cases can be handled by pretending that the Receiver is being
					//	clocked by a source that is equivalent to the clock source for the TxC pin.
					case XC_BRG0:
						tree_A = CLK_BRG0;
						goto setup_rx_clk;
					case XC_BRG1:
						tree_A = CLK_BRG1;
						goto setup_rx_clk;
					case TXC_CTR1:
						tree_A = CLK_CTR1;
						goto setup_rx_clk;
					case XC_DPLL:
						tree_A = CLK_DPLL;
						goto setup_rx_clk;
					case TXC_PORT1:
						tree_A = CLK_EXT_REF;
						goto setup_rx_clk;
					default:
						// bad setting
						return (TX_ERROR + PARAM_ERROR + BAD_clk_pin);
						break;
				}
				break;

			case CLK_DPLL:
				//	Receiver clocked from DPLL
				// Set the DPLL divisor
				if (!validate)
					api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~DPLL_DIV) | dpll_div_set(set->rxclk.dpll_div));
				//	set the DPLL clocking source
				switch (set->rxclk.clk_tree.B) {
					case DPLL_BRG0:
					case DPLL_BRG1:
					case DPLL_RXC:
					case DPLL_TXC:
						if (!validate)
							api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~DPLL_SRC) | DPLL_SRC_SET(set->rxclk.clk_tree.B));
						break;
					default:
						return (RX_ERROR + PARAM_ERROR + BAD_clk_treeB);
						break;
				}

				// set the DPLL mode
				switch (set->rxclk.enc_dec) {
					case NRZI_MARK:
					case NRZI_SPACE:
						if (!validate)
							api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~DPLL_MODE) | DPLL_NRZ);
						break;
					case BIPH_MARK:
					case BIPH_SPACE:
						if (!validate)
							api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~DPLL_MODE) | DPLL_BI_MS);
						break;
					case BIPH_LEVEL:
					case BIPH_DIFF:
						if (!validate)
							api->write_reg_word((void *)&iusc->HCR.WORD,  (api->read_reg_word((void *)&iusc->HCR.WORD) & ~DPLL_MODE) | DPLL_BIM);
						break;
					default:
						//something is wrong with the Receiver decoding setting
						return (RX_ERROR + PARAM_ERROR + BAD_enc_dec);
						break;
				}
				// determine if the DPLL clocking is from either BRG clock source
				if ((set->rxclk.clk_tree.B != DPLL_BRG0) && (set->rxclk.clk_tree.B != DPLL_BRG1)) {
					// No, the clocking is from some other (RxC or TxC) source
					break;
				}
				// fall through to next case

			case CLK_BRG0:
			case CLK_BRG1:
				// setup the BRG and CTR divisor settings as appropriate
				if ((temp = setup_brg_ctr (set, (void*)iusc, api, FOR_RECEIVER, tree_A, validate, &resource)) != SUCCESS) {
					// there was a failure in the setup
					return (temp);
				}
				break;

			case CLK_CTR0:
			case CLK_CTR1:
				if ((temp = setup_ctr((void*)iusc, api, tree_A, set->rxclk.ctr_div, set->rxclk.clk_tree.D, 0, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			default:
				// something is wrong with the Receiver clocking source setting
				return (RX_ERROR + PARAM_ERROR + BAD_clk_treeA);
				break;
		}

		// set the clocking source for the Receiver
		tree_A = set->rxclk.clk_tree.A;
		tree_A &= 0x7F;		// remove the bit that indicates a bypassed CTR
		if (!validate)
			api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~RXCLK_SRC) | RXCLK_SRC_SET(tree_A));

		//------------------------------------------------------------
		// Now the Transmitter
		//	The code above, for the Receiver clocking setup, may have used clocking resources that will be
		//	requested for the Transmitter. This is allowed, but the settings are required to be the
		//	same for the Transmitter. The code below will check if there are collisions or violations
		//	in the use of the clocking resourses.

		tree_A = set->txclk.clk_tree.A;

		// The code may loop back to this point under certain clocking situations
		setup_tx_clk:
		switch (tree_A) {
			case CLK_INT_REF:
			case CLK_EXT_REF:
				// In order to use the INT or EXT reference, the CTR by-pass must be enabled
				//	(and the by-pass affects both counters), but is the Receiver using either CTR?
				if ((temp = setup_ctr((void*)iusc, api, 0,0,0,1, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				// fall through to next case

			case CLK_NONE:
				break;

			case CLK_RXC:
				// Determine if RxC is driven from an internal source
				//	Some of the possible sources produce a "circular" clock structure
				//	which would be non-functional.
				switch (set->rxclk.clk_pin) {
					case XC_INP:
						// The source for the RxC pin is an external signal
						break;
					case RXC_RXCLK:
					case XC_CHAR_CLK:
					case RXC_RXSYNC:
						// These combinations might be circular if the Receiver is
						//	also clocked from the RxC pin
						if (set->rxclk.clk_tree.A == CLK_RXC) {
							return (TX_ERROR + INVALID_ERROR + CIRCULAR_clocking);
						}
					// These next 4 cases can be handled by pretending that the Transmitter is being
					//	clocked by a source that is equivalent to the clock source for the RxC pin.
					case XC_BRG0:
						tree_A = CLK_BRG0;
						goto setup_tx_clk;
					case XC_BRG1:
						tree_A = CLK_BRG1;
						goto setup_tx_clk;
					case RXC_CTR0:
						tree_A = CLK_CTR0;
						goto setup_tx_clk;
					case XC_DPLL:
						tree_A = CLK_DPLL;
						goto setup_tx_clk;
					default:
						// bad setting
						return (RX_ERROR + PARAM_ERROR + BAD_clk_pin);
						break;
				}
				break;

			case CLK_TXC:
				// determine if TxC is driven from an internal source
				//	Some of the possible sources produce a "circular" clock structure
				//	which would be non-functional.
				switch (set->txclk.clk_pin) {
					case XC_INP:
						// The source for the TxC pin is an external signal
						break;
					case TXC_TXCLK:
					case XC_CHAR_CLK:
					case TXC_TXCOMP:
						// These cases are "circular" clocking, which yields a non-functional setup
						return (TX_ERROR + INVALID_ERROR + CIRCULAR_clocking);
					// These next 4 cases can be handled by pretending that the Receiver is being
					//	clocked by a source that is equivalent to the clock source for the TxC pin.
					case XC_BRG0:
						tree_A = CLK_BRG0;
						goto setup_tx_clk;
					case XC_BRG1:
						tree_A = CLK_BRG1;
						goto setup_tx_clk;
					case TXC_CTR1:
						tree_A = CLK_CTR1;
						goto setup_tx_clk;
					case XC_DPLL:
						tree_A = CLK_DPLL;
						goto setup_tx_clk;
					default:
						// bad setting
						return (TX_ERROR + PARAM_ERROR + BAD_clk_pin);
						break;
				}
				break;

			case CLK_DPLL:
				// There's an expectation that the Receiver is also setup to use
				//	the DPLL, otherwise there's not really any point to clocking
				//	the Transmitter from the DPLL. The Transmitter will be set to
				//	clock from the DPLL below
				if (set->rxclk.clk_tree.A != CLK_DPLL) {
					return (TX_ERROR + INVALID_ERROR + DPLL_inconsistent);
				}
				break;
			case CLK_BRG0:
			case CLK_BRG1:
				// setup the BRG and CTR divisor settings as appropriate
				if ((temp = setup_brg_ctr (set, (void*)iusc, api, FOR_TRANSMITTER, tree_A, validate, &resource)) != SUCCESS) {
					// there was a failure in the setup
					return (temp);
				}
				break;

			case CLK_CTR0:
			case CLK_CTR1:
				if ((temp = setup_ctr((void*)iusc, api, tree_A, set->txclk.ctr_div, set->txclk.clk_tree.D, 0, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			default:
				// something is wrong with the Transmitter clocking source setting
				return (TX_ERROR + PARAM_ERROR + BAD_clk_treeA);
				break;
		}

		// set the clocking source for the Transmitter
		tree_A = set->txclk.clk_tree.A;
		tree_A &= 0x7F;		// remove the bit that indicates a bypassed CTR
		if (!validate)
			api->write_reg_word((void *)&iusc->CMCR.WORD,  (api->read_reg_word((void *)&iusc->CMCR.WORD) & ~TXCLK_SRC) | TXCLK_SRC_SET(tree_A));

		//------------------------------------------------------------
		// And finally the RxC and TxC clocks

		//	First the RxC pin
		tree_A = set->rxclk.clk_pin;
		switch (tree_A) {
			case XC_INP:
				break;
			case RXC_RXCLK:
			case XC_CHAR_CLK:
			case RXC_RXSYNC:
				// If the Receiver is clocked from the RxC pin, then
				//	this creates a circular clock structure, which won't work
				if (set->rxclk.clk_tree.A == CLK_RXC) {
					return (GENERAL_ERROR + INVALID_ERROR + CIRCULAR_clocking);
				}
				break;
			case XC_BRG0:
			case XC_BRG1:
				// Check to see if the BRG-N was programmed previously when either
				//	the Receiver or Transmitter were setup
				if (resource.used_brg_src[(tree_A & 0x01)] == 0) {
					// Not setup yet
					//	To get to this point in the code, BOTH the Receiver and Transmitter
					//	must have been setup to be clocked from the either the RxC or TxC pins, and
					//	since the RxC pin is being setup to be an output from BRG-0, then there
					//	is a conflict about which set a parameters use for the setup of BRG-0
					//
					//	Since this is the setup for the RxC pin, then use the Receiver setup parameters
					if ((temp = setup_brg_ctr (set, (void*)iusc, api, FOR_RECEIVER, tree_A, validate, &resource)) != SUCCESS) {
						// there was a failure in the setup
						return (temp);
					}
				}
				break;

			case RXC_CTR0:
				if ((temp = setup_ctr((void*)iusc, api, 0, set->rxclk.ctr_div, set->rxclk.clk_tree.D, 0, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			case RXC_PORT0:
				if ((temp = setup_ctr((void*)iusc, api, 0, 0, 0, 1, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			case XC_DPLL:
				// There's an expectation that the Receiver must also be setup to use the DPLL
				if (set->rxclk.clk_tree.A != CLK_DPLL) {
					return (GENERAL_ERROR + INVALID_ERROR + DPLL_inconsistent);
				}
				break;
			default:
				// bad setting
				return (RX_ERROR + PARAM_ERROR + BAD_clk_pin);
				break;
		}

		// Setup RxC pin mode
		if (!validate)
			api->write_reg_word((void *)&iusc->IOCR.WORD,  (api->read_reg_word((void *)&iusc->IOCR.WORD) & ~RXC_MODE) | RXC_MODE_SET(set->rxclk.clk_pin));

		//	Now the TxC pin
		tree_A = set->txclk.clk_pin;
		switch (tree_A) {
			case XC_INP:
				break;
			case TXC_TXCLK:
			case XC_CHAR_CLK:
			case TXC_TXCOMP:
				// If the Transmitter is clocked from the TxC pin, then
				//	this creates a circular clock structure, which won't work
				if (set->txclk.clk_tree.A == CLK_TXC) {
					return (GENERAL_ERROR + INVALID_ERROR + CIRCULAR_clocking);
				}
				break;
			case XC_BRG0:
			case XC_BRG1:
				// Check to see if the BRG-N was programmed previously when either
				//	the Receiver or Transmitter or RxC pin were setup
				if (resource.used_brg_src[(tree_A & 0x01)] == 0) {
					// Not setup yet
					//	To get to this point in the code, BOTH the Receiver and Transmitter
					//	must have been setup to be clocked from the either the RxC or TxC pins, and
					//	since the RxC pin is being setup to be an output from BRG-0, then there
					//	is a conflict about which set a parameters use for the setup of BRG-0
					//
					//	Since this is the setup for the TxC pin, then use the Transmitter setup parameters
					if ((temp = setup_brg_ctr (set, (void*)iusc, api, FOR_TRANSMITTER, tree_A, validate, &resource)) != SUCCESS) {
						// there was a failure in the setup
						return (temp);
					}
				}
				break;

			case TXC_CTR1:
				if ((temp = setup_ctr((void*)iusc, api, 1, set->txclk.ctr_div, set->txclk.clk_tree.D, 0, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			case TXC_PORT1:
				if ((temp = setup_ctr((void*)iusc, api, 0, 0, 0, 1, validate, &resource)) != SUCCESS) {
					return (temp);
				}
				break;

			case XC_DPLL:
				// There's an expectation that the Receiver must also be setup to use the DPLL
				if (set->rxclk.clk_tree.A != CLK_DPLL) {
					return (-1);
				}
				break;
			default:
				// bad setting
				return (TX_ERROR + PARAM_ERROR + BAD_clk_pin);
				break;
		}

		// Setup TxC pin mode
		if (!validate)
			api->write_reg_word((void *)&iusc->IOCR.WORD,  (api->read_reg_word((void *)&iusc->IOCR.WORD) & ~TXC_MODE) | TXC_MODE_SET(set->txclk.clk_pin));

		return (SUCCESS);
	}



	//**************************************************************************
	//**************************************************************************
	//	Setup a Port (IUSC device and surrounding circuits)
	//		Args:
	//			'set'			a pointer to a settings structure
	//			'assert_reset'	1= complete setup (reset)
	//							0= setup without reset
	//			validate		1= validate the setup (without touching the hardware)
	//							0= normal setup
	//		Return:
	//			0	=	success
	//			>0	=	failure code (see codes at top of this file)
	//
#ifdef QNX4
	int iusc_port_setup (BHN_PortSettings *set, void __far *v_iusc, void __far *v_pld, void *context, char assert_reset, char validate)
#else
	int iusc_port_setup (BHN_PortSettings *set, void *v_iusc, void *v_pld, void *context, char assert_reset, char validate)
#endif

	{
		int temp;
		unsigned short reg1, reg2, tx_enable, rx_enable;
		unsigned char port = set->port;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif


		// Is the port number out of range?
		if (port >= N_SYNC_PORTS) {
			return (GENERAL_ERROR + PARAM_ERROR + BAD_port);
		}


		if (!validate) {
			if (assert_reset) {
				set_pld_int(0, (void*)pld, context);		// disable interrupt to Coldfire
				iusc_reset (port,(void*)iusc, (void*)pld, context);
			}
			else {
				//	reset serial controller
				api->write_reg_word((void *)&iusc->CCAR.WORD, RTRESET);
				temp=100; while (temp--);
				api->write_reg_word((void *)&iusc->CCAR.WORD,  0);
			}


			//------------------------------------------------------------
			//	Stop the Receiver and Transmitter
			api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD) & ~TR_ENAB) | TR_DIS_IMM);
			api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~TR_ENAB) | TR_DIS_IMM);

			//	Clear any previous receiver and transmitter status's
			api->write_reg_word((void *)&iusc->RCSR.WORD, UNLATCH_RCSR_ALL);
			api->write_reg_char((void *)&iusc->TCSR.byte.LSB, UNLATCH_TCSR_ALL);

			//------------------------------------------------------------
			// Setup the "Port" pins of the IUSC device
			api->write_reg_word((void *)&iusc->PCR.WORD, P0_CLK0 | P1_CLK1 | P2_HIGH | P3_GPI | P4_LOW | P5_LOW | P6_GPI | P7_LOW);
			// Setup the "IO" pins of the IUSC device
			api->write_reg_word((void *)&iusc->IOCR.WORD,  RM_RXC_INP | TM_TXC_INP | TXD_TXD | RXR_INP | TXR_INP | DCD_INP | CTS_INP);
		}

		//------------------------------------------------------------
		// Setup the Line Interface mode
		reg1 = 0;
		if ((set->line_mode & 0x07) > LM_SHUTDOWN) {
			return (GENERAL_ERROR + PARAM_ERROR + BAD_line_mode);
		}
		if (set->line_mode & LM_TERM) reg1 |= TERM_ENAB;
		else reg1 &= ~TERM_ENAB;
		reg1 = (reg1 & ~LINE_MODE) | SET_LINE_MODE(set->line_mode);

		// Setup Duplex mode
		switch (set->duplex_mode) {
			case FOUR_WIRE_RTS:
			case FOUR_WIRE_TXCOMP:
				reg1 |= DUP_FOUR_WIRE;
				// fall into next case
			case TWO_WIRE_RTS:
			case TWO_WIRE_TXCOMP:
				reg1 |= DUP_24;
				break;
			case DUPLEX_FULL:
				break;
			default:
				return (GENERAL_ERROR + PARAM_ERROR + BAD_duplex_mode);
				break;
		}

		// check if Receiver hardware flow control (using RTS) is enabled, along with something other than Full-Duplex
		if (set->async_settings.hflow & 0x80) {
			if (set->duplex_mode != DUPLEX_FULL) {
				return (GENERAL_ERROR | INVALID_ERROR | HFLOW_DUPLEX_incompatible);
			}
		}


		if (!validate) {
			if (set->duplex_mode & DUPLEX_TX_COMP) {
				// setup the IUSC Port-7 pin to be the "TX Complete" signal
				api->write_reg_word((void *)&iusc->PCR.WORD,  (api->read_reg_word((void *)&iusc->PCR.WORD) & ~P7MODE) | P7_TXCOMP);
			}
			write_lifc (port, v_pld, context, reg1);
		}


		//------------------------------------------------------------
		//	Setup IUSC TX and RX clocking
		if ((temp = setup_iusc_clk (set, v_iusc, v_pld, context, validate)) != 0) {
			return (temp);
		}

		//=====================================================================
		//	Setup TX and RX Serial Mode & Submodes

		//------------------------------------------------------------
		//	First some pre-setup
		if (!validate)
			api->write_reg_word((void *)&iusc->CMR.WORD, 0);

		//------------------------------------------------------------
		//	First Transmitter
		switch (set->smode.tx) {
			// These settings are NOT supported
			case SMODE_ISO:
			case SMODE_TRANS_BI:
			case SMODE_9BIT:
				// Setting not supported (yet)
				return (TX_ERROR + INVALID_ERROR + BAD_smode);
				break;
			// These settings are supported
			case SMODE_ASYNC:
			case SMODE_MONO:
			case SMODE_BISYNC:
			case SMODE_HDLC:
				break;
			default:
				//	setting not valid for Transmitter
				return (TX_ERROR + PARAM_ERROR + BAD_smode);
				break;
		}
		// set the basic mode
		if (!validate)
			api->write_reg_word((void *)&iusc->CMR.WORD,  (api->read_reg_word((void *)&iusc->CMR.WORD) & ~TX_MODE) | TX_MODE_SET(set->smode.tx));

		// Prepare sub-mode setting bits
		reg1 = 0;	//	this value will eventually be written to the CMR register
		switch (set->smode.tx) {
			case SMODE_ASYNC:
				// clocks per bit
				switch (set->txclk.async_div) {
					case 16:
						reg1 |= TX_X16;
						break;
					case 32:
						reg1 |= TX_X32;
						break;
					case 64:
						reg1 |= TX_X64;
						break;
					default:
						// unknown setting
						return (TX_ERROR + PARAM_ERROR + BAD_async_div);
						break;
				}

				// stop bits (the setting is in units of 1/16 of a bit)
				temp = set->tx_frac_stop;
				switch (temp) {
					case 16:
						reg1 |= TX_1_STOP;
						break;
					case 32:
						reg1 |= TX_2_STOP;
						break;
					default:
						// all other settings
						if ((temp > 32) || (temp < 9)) {
							// these values are illegal
							return (TX_ERROR + PARAM_ERROR + BAD_tx_frac_stop);
						}
						if (temp <= 15) {
							// settings 9 to 15
							reg1 |= TX_1_SHAVED;
							if (!validate)
								api->write_reg_word((void *)&iusc->CCR.WORD,  (api->read_reg_word((void *)&iusc->CCR.WORD) & ~TXSHAVEL) | SET_TXSHAVE(temp - 1));
						}
						else if (temp >= 17) {
							// settings 17 to 31
							reg1 |= TX_2_SHAVED;
							if (!validate)
								api->write_reg_word((void *)&iusc->CCR.WORD,  (api->read_reg_word((void *)&iusc->CCR.WORD) & ~TXSHAVEL) | SET_TXSHAVE(temp - 17));
						}
						break;
				}
				break;	//end, case SMODE_ASYNC:


			case SMODE_BISYNC:
				// choose SYNC character pairs in Bi-Sync mode
				reg1 |= CLOSE_SYN01_TSR;
				// fall through

			case SMODE_MONO:
				// CRC on TX underrun
				if(set->crc.tx_type != CRC_NONE) {
					switch (set->crc.tx_mode & ~CRC_TX_EOFM) {
						case CRC_TX_UNDERRUN:
							reg1 |= CRC_UNDERUN;
							break;
						default:
							return (TX_ERROR + PARAM_ERROR + BAD_crc_mode);
							break;
					}
				}

				// TX preamble enable
				if (set->tx_pre_idle.pre_pat > 0) {
					reg1 |= PREAMBLE;	// enable preamble

					//	select preamble style
					reg2 = 0;
					switch (set->tx_pre_idle.pre_pat) {
						case PREAM_ZEROS:
							reg2 |= PREAMBLE_ZEROS;
							break;
						case PREAM_ONES:
							reg2 |= PREAMBLE_ONES_FLAGS;
							break;
						case PREAM_FLAGS:
							//	Applies to HDLC mode only
							return (TX_ERROR + INVALID_ERROR + BAD_pre_pat);
							break;
						case PREAM_ZERO_ONE:
							reg2 |= PREAMBLE_01;
							break;
						case PREAM_ONE_ZERO:
							reg2 |= PREAMBLE_10;
							break;
						default:
							// illegal selection
							return (TX_ERROR + PARAM_ERROR + BAD_pre_pat);
							break;
					}

					// set preamble length
					switch (set->tx_pre_idle.pre_len) {
						case 8:
							reg2 |= PREAMBLE_8;
							break;
						case 16:
							reg2 |= PREAMBLE_16;
							break;
						case 32:
							reg2 |= PREAMBLE_32;
							break;
						case 64:
							reg2 |= PREAMBLE_64;
							break;
						default:
							// illegal setting
							return (TX_ERROR + PARAM_ERROR + BAD_pre_len);
							break;
					}
					if (!validate)
						api->write_reg_word((void *)&iusc->CCR.WORD, (api->read_reg_word((void *)&iusc->CCR.WORD) & ~(FLAG_PREAMBLE | TXPREL | TXPREPAT)) | reg2);
				}

				// Sync character length
				switch (set->sync_addr.tx_len) {
					case 0:
						reg1 |= TX_SYNCS_TXLEN;
						break;
					case 8:
						reg1 |= TX_SYNCS_8;
						break;
					default:
						// all other are illegal
						return (TX_ERROR + PARAM_ERROR + BAD_sync_addr_len);
						break;
				}
				break;	//end, case SMODE_BISYNC: or SMODE_MONO:

			// HDLC-SDLC mode
			case SMODE_HDLC:
				// TX underrun behaviour
				if (set->crc.tx_type != CRC_NONE) {
					if ((set->crc.tx_mode & ~CRC_TX_EOFM) == CRC_TX_UNDERRUN) {
						reg1 |= CRC_FLAG;
					}
					else {
						switch (set->hdlc.tx_underrun) {
							case HDLC_UNDERRUN_ABORT_7:
								reg1 |= ABORT_7;
								break;
							case HDLC_UNDERRUN_ABORT_15:
								reg1 |= ABORT_15;
								break;
							case HDLC_UNDERRUN_FLAG:
								reg1 |= ABORT_FLAG;
								break;
							default:
								return (TX_ERROR + PARAM_ERROR + BAD_tx_underrun);
								break;
						}
					}
				}
				// TX preamble enable (This enables the "Minimum number of FLAGS between Frames" mode)
				if (set->tx_pre_idle.pre_pat > 0) {
					reg1 |= PREAMBLE;	// enable preamble

					//	select preamble style
					reg2 = 0;
					switch (set->tx_pre_idle.pre_pat) {
						case PREAM_ZEROS:
						case PREAM_ONES:
						case PREAM_ZERO_ONE:
						case PREAM_ONE_ZERO:
							// illegal combination
							return (TX_ERROR + INVALID_ERROR + BAD_pre_pat);
							break;
						case PREAM_FLAGS:
							reg2 |= PREAMBLE_ONES_FLAGS;
							reg2 |= SEND_FLAGS_AS_PREAMBLE;
							break;
						default:
							// illegal selection
							return (TX_ERROR + PARAM_ERROR + BAD_pre_pat);
							break;
					}

					// set preamble length (This sets the minimum number of FLAGS between Frames)
					switch (set->tx_pre_idle.pre_len) {
						case 8:
							reg2 |= PREAMBLE_8;		//	3 FLAGS (including opening and closing FLAGS)
							break;
						case 16:
							reg2 |= PREAMBLE_16;	//	4 FLAGS
							break;
						case 32:
							reg2 |= PREAMBLE_32;	//	6 FLAGS
							break;
						case 64:
							reg2 |= PREAMBLE_64;	//	10 FLAGS
							break;
						default:
							// illegal setting
							return (TX_ERROR + PARAM_ERROR + BAD_pre_len);
							break;
					}
					if (!validate)
						api->write_reg_word((void *)&iusc->CCR.WORD,  (api->read_reg_word((void *)&iusc->CCR.WORD) & ~(FLAG_PREAMBLE | TXPREL | TXPREPAT)) | reg2);
				}

				// Shared Zero bit in consecutive FLAGS
				if (set->hdlc.tx_flag_share_zero) {
					reg1 |= FLAGS_SHARE_0;
				}

				break;	//end, case SMODE_HDLC:
		}
		// set the TX sub-mode field
		if (!validate)
			api->write_reg_word((void *)&iusc->CMR.WORD, ( api->read_reg_word((void *)&iusc->CMR.WORD) & ~TX_SUB_MODE) | reg1);

		//------------------------------------------------------------
		//	Now Receiver
		switch (set->smode.rx) {
			// These settings are NOT supported
			case SMODE_EXT_SYNC:
			case SMODE_ISO:
			case SMODE_TRANS_BI:
			case SMODE_9BIT:
				// Setting not supported (yet)
				return (RX_ERROR + INVALID_ERROR + BAD_smode);
				break;
			// These settings are supported
			case SMODE_ASYNC:
			case SMODE_MONO:
			case SMODE_BISYNC:
			case SMODE_HDLC:
				break;
			default:
				//	setting not valid for Receiver
				return (RX_ERROR + PARAM_ERROR + BAD_smode);
				break;
		}
		// set the basic mode
		if (!validate)
			api->write_reg_word((void *)&iusc->CMR.WORD,  (api->read_reg_word((void *)&iusc->CMR.WORD) & ~RX_MODE) | RX_MODE_SET(set->smode.rx));
		// Prepare sub-mode setting bits
		reg1 = 0;
		switch (set->smode.rx) {
			case SMODE_ASYNC:
				// clocks per bit
				switch (set->rxclk.async_div) {
					case 16:
						reg1 |= RX_X16;
						break;
					case 32:
						reg1 |= RX_X32;
						break;
					case 64:
						reg1 |= RX_X64;
						break;
					default:
						// not legal
						return (RX_ERROR + PARAM_ERROR + BAD_async_div);
						break;
				}
				break;	//end, case SMODE_ASYNC:

			case SMODE_MONO:
			case SMODE_BISYNC:
				// Strip sync chars from being fifo'd (or counted in CRC)?
				if (set->sync_addr.strip_sync) {
					reg1 |= STRIP_SYNC;
				}

				// Sync character length
				switch (set->sync_addr.rx_len) {
					case 0:
						reg1 |= RX_SYNCS_RXLEN;
						break;
					case 8:
						reg1 |= RX_SYNCS_8;
						break;
					default:
						// not legal
						return (RX_ERROR + PARAM_ERROR + BAD_sync_addr_len);
						break;
				}
				break;	//end, case SMODE_MONO: or SMODE_BISYNC:

			case SMODE_HDLC:
				switch (set->hdlc.rx_addr_control) {
					case HDLC_RX_NO_ADDR_CONT:
					case HDLC_RX_ADDR1_CONT0:
					case HDLC_RX_ADDR1_CONT1:
					case HDLC_RX_ADDR1_CONT2:
					case HDLC_RX_EXADDR_CONT1:
					case HDLC_RX_EXADDR_CONT2:
					case HDLC_RX_EXADDR_CONT_GE2:
					case HDLC_RX_EXADDR_CONT_GE3:
						reg1 |= (set->hdlc.rx_addr_control << 4);
						break;
					default:
						return (RX_ERROR + PARAM_ERROR + BAD_hdlc_rx_addr_cont);
						break;
				}
				break;	//end, case SMODE_HDLC:
		}
		// set the RX sub-mode field
		if (!validate)
			api->write_reg_word((void *)&iusc->CMR.WORD,  (api->read_reg_word((void *)&iusc->CMR.WORD) & ~RX_SUB_MODE) | reg1);
		//=====================================================================
		//	Now that the TX and RX Mode and Sub-Modes are setup, the other
		//		settings can be performed in any order.

		// Transmitter IDLE pattern
		switch (set->tx_pre_idle.tx_idle) {
			case IDLE_ALT01:
			case IDLE_ZEROS:
			case IDLE_ONES:
			case IDLE_MRK_SPC:
			case IDLE_SPACE:
			case IDLE_MARK:
				if (set->smode.tx == SMODE_ASYNC) {
					//	above IDLE patterns not valid for ASYNC mode
					return (TX_ERROR + INVALID_ERROR + BAD_tx_idle);
				}
			case IDLE_DEF:
				if (!validate)
					api->write_reg_word((void *)&iusc->TCSR.WORD,  (api->read_reg_word((void *)&iusc->TCSR.WORD) & ~TXIDLE) | TXIDLE_SET(set->tx_pre_idle.tx_idle));
				break;
			default:
				//	not legal
				return (TX_ERROR + PARAM_ERROR + BAD_tx_idle);
				break;
		}

		// Transmitter Encoding and Receiver Decoding
		//	Receiver
		switch (set->rxclk.enc_dec) {
			case NRZI_MARK:
			case NRZI_SPACE:
			case BIPH_MARK:
			case BIPH_SPACE:
			case BIPH_LEVEL:
			case BIPH_DIFF:
				//	check for ASYNC mode
				if (set->smode.rx == SMODE_ASYNC) {
					//	above encoding values not valid for ASYNC mode
					return (RX_ERROR + INVALID_ERROR + BAD_enc_dec);
				}
				// check if the RX clocking is set correctly
				if (set->rxclk.clk_tree.A != CLK_DPLL) {
					return (RX_ERROR + INVALID_ERROR + BAD_enc_dec);
				}
				// fall through
			case NRZ:
			case NRZ_INV:
				if (!validate)
					api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD)& ~RX_DECODE) | TR_ENC(set->rxclk.enc_dec));
				break;
			default:
				// all others are illegal
				return (RX_ERROR + PARAM_ERROR + BAD_enc_dec);
				break;
		}
		// Transmitter
		switch (set->txclk.enc_dec) {
			case NRZI_MARK:
			case NRZI_SPACE:
			case BIPH_MARK:
			case BIPH_SPACE:
			case BIPH_LEVEL:
			case BIPH_DIFF:
				//	check for ASYNC mode
				if (set->smode.tx == SMODE_ASYNC) {
					//	above encoding values not valid for ASYNC mode
					return (TX_ERROR + INVALID_ERROR + BAD_enc_dec);
				}
				// check if the RX clocking is set correctly
				//	(this appears to be an error here, but it is not,
				//		checking that the RX clock is set to DPLL is intentional)
				if (set->rxclk.clk_tree.A != CLK_DPLL) {
					return (RX_ERROR + INVALID_ERROR + BAD_enc_dec);
				}
				// fall through
			case NRZ:
			case NRZ_INV:
				if (!validate)
					api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~TX_ENCODE) | TR_ENC(set->txclk.enc_dec));
				break;
			default:
				// all others are illegal
				return (TX_ERROR + PARAM_ERROR + BAD_enc_dec);
				break;
		}

		//	Sync Characters, OR HDLC Address
		reg1 = 0;
		switch (set->smode.tx) {
			case SMODE_BISYNC:
				reg1 |= SET_SYN1(set->sync_addr.tx1);
				// fall through
			case SMODE_MONO:
				reg1 |= SET_SYN0(set->sync_addr.tx0);
				break;
			default:
				// all other modes do not use SYNC charaters (or HDLC address)
				break;
		}
		if (!validate) api->write_reg_word((void *)&iusc->TSR.WORD,   reg1);


		reg1 = 0;
		switch (set->smode.rx) {
			case SMODE_BISYNC:
				reg1 |= SET_SYN1(set->sync_addr.rx1);
				reg1 |= SET_SYN0(set->sync_addr.rx0);
				break;
			case SMODE_MONO:
				reg1 |= SET_SYN1(set->sync_addr.rx0);	// SYN0 is placed in the SYN1 location
				break;
			case SMODE_HDLC:
				reg1 |= SET_SYN0(set->sync_addr.rx0);	//	this is the HDLC address
				break;
			default:
				break;
		}
		if (!validate) api->write_reg_word((void *)&iusc->RSR.WORD, reg1);

		//	Number of Data bits
		if ((set->dbits.tx > 8) || (set->dbits.tx < 1)) {
			return (TX_ERROR + PARAM_ERROR + BAD_dbits);
		}
		if ((set->dbits.rx > 8) || (set->dbits.rx < 1)) {
			return (RX_ERROR + PARAM_ERROR + BAD_dbits);
		}
		if (!validate)
			api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~TR_LENGTH) | TR_N(set->dbits.tx));
		if (!validate)
			api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD) & ~TR_LENGTH) | TR_N(set->dbits.rx));




		// Parity selection
		//	Transmitter
		switch (set->parity.tx) {
			case PAR_EVEN:
			case PAR_ODD:
			case PAR_SPACE:
			case PAR_MARK:
				if (!validate)
					api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~PAR_TYPE) | TR_PAR(set->parity.tx - 1) | PAR_ENAB);
				break;
			case PAR_NONE:
				if (!validate)
					api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~PAR_ENAB));
				break;
			default:
				// illegal setting
				return (TX_ERROR + PARAM_ERROR + BAD_parity);
				break;
		}
		//	Receiver
		switch (set->parity.rx) {
			case PAR_EVEN:
			case PAR_ODD:
			case PAR_SPACE:
			case PAR_MARK:
				if (!validate)
					api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD) & ~PAR_TYPE) | TR_PAR(set->parity.rx) | PAR_ENAB);
				break;
			case PAR_NONE:
				if (!validate)
					api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD) & ~PAR_ENAB));
				break;
			default:
				// illegal setting
				return (RX_ERROR + PARAM_ERROR + BAD_parity);
				break;
		}

		//	Remaining CRC setups
		//	Transmitter
		if (set->crc.tx_type != CRC_NONE) {
			switch (set->crc.tx_start) {
				case CRC_START_0:
					if (set->smode.tx == SMODE_HDLC) {
						return (TX_ERROR + INVALID_ERROR + BAD_crc_start);
					}
					// fall through
				case CRC_START_1:
					// crc.tx_start used below
					break;
				default:
					return (TX_ERROR + PARAM_ERROR + BAD_crc_start);
					break;
			}
		}
		switch (set->crc.tx_type) {
			case CRC16:
				if (set->smode.tx == SMODE_HDLC) {
					return (TX_ERROR + INVALID_ERROR + BAD_crc_type);
				}
				//fall through
			case CCITT:
			case CRC32:
				if (!validate) {
					reg1 = api->read_reg_word((void *)&iusc->TMR.WORD);
					reg1 &= ~(CRC_START | CRC_TYPE | TX_CRC_END);
					reg1 |= CRC_START_SET(set->crc.tx_start) | CRC_TYPE_SET(set->crc.tx_type) | CRC_ENAB;
					if (set->crc.tx_mode & CRC_TX_EOFM) reg1 |= TX_CRC_END;
					api->write_reg_word((void *)&iusc->TMR.WORD,  reg1);
				}
				break;
			case CRC_NONE:
				if (!validate)
					api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD) & ~CRC_ENAB));
				break;
			default:
				//	illegal setting
				return (TX_ERROR + PARAM_ERROR + BAD_crc_type);
				break;
		}

		//Receiver
		if (set->crc.rx_type != CRC_NONE) {
			switch (set->crc.rx_start) {
				case CRC_START_0:
					if (set->smode.rx == SMODE_HDLC) {
						return (RX_ERROR + INVALID_ERROR + BAD_crc_start);
					}
				case CRC_START_1:
					// crc.rx_start used below
					break;
				default:
					return (RX_ERROR + PARAM_ERROR + BAD_crc_start);
					break;
			}
		}
		switch (set->crc.rx_type) {
			case CRC16:
				if (set->smode.rx == SMODE_HDLC) {
					return (RX_ERROR + INVALID_ERROR + BAD_crc_type);
				}
				//fall through
			case CCITT:
			case CRC32:
				if (!validate) {
					reg1 = api->read_reg_word((void *)&iusc->RMR.WORD);
					reg1 &= ~(CRC_START | CRC_TYPE);
					reg1 |= CRC_START_SET(set->crc.rx_start) | CRC_TYPE_SET(set->crc.rx_type) | CRC_ENAB;
					api->write_reg_word((void *)&iusc->RMR.WORD,  reg1);
				}
				break;
			case CRC_NONE:
				if (!validate)
					api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD)& ~CRC_ENAB));
				break;
			default:
				//	illegal setting
				return (RX_ERROR + PARAM_ERROR + BAD_crc_type);
				break;
		}

		// Assume that the TX enable will be "normal"
		//	this value may be altered below if necessary
		tx_enable = TR_ENAB_UNCON;

		// Get the current setting of the LIFC for this port
		reg1 = read_lifc(port, (void*)pld, context);

		// check if Transmitter hardware flow control (using CTS) is enabled,
		//	along with the setting which enables triggered/gated Transmission
		if (set->async_settings.hflow & 0x08) {
			//	Change the Transmitter to use the CTS signal to enable transmission
			tx_enable = TR_ENAB_CTS;
		}


		// Assume that the RX enable will be "normal"
		//	this value may be altered below if necessary
		rx_enable = TR_ENAB_UNCON;


		if (!validate) {
			// write out the new LIFC control value
			write_lifc(port, v_pld, context, reg1);

			//	enable the Transmitter
			api->write_reg_word((void *)&iusc->TMR.WORD,  (api->read_reg_word((void *)&iusc->TMR.WORD)& ~TR_ENAB) | tx_enable);
			//	enable the Receiver
			api->write_reg_word((void *)&iusc->RMR.WORD,  (api->read_reg_word((void *)&iusc->RMR.WORD) & ~TR_ENAB) | rx_enable);
		}

		return (SUCCESS);
	}



	//**************************************************************************
	//	Setting the bit rate is not as easy as it would seem, the myriad of clocking
	//	sources and settings available in the IUSC make it difficult to determine
	//	what things to change to cause a bit rate change.
	//
	//	This function can only be called sometime AFTER the above 'iusc_port_setup' function
	//	has been called.
	//
	//		args:
	//			'port'			0 to N-1
	//			'target'		indicates whether the setup is for the Receiver or Transmitter
	//			'bps'			Bit rate (integer portion)
	//			'bps_frac'		Bit rate (fractional portion, in 1/10 units)
	//		return:
	//			0	=	success
	//			>0	=	failure code
	//
#ifdef QNX4
	int iusc_set_bit_rate (BHN_PortSettings *set, void __far *v_iusc, void __far *v_pld, void *context, int target, unsigned long bps, unsigned char bps_frac, struct _resource *resource)
#else
	int iusc_set_bit_rate (BHN_PortSettings *set, void *v_iusc, void *v_pld, void *context, int target, unsigned long bps, unsigned char bps_frac, struct _resource *resource)
#endif

	{
		_trclk *trclk;
		unsigned long save_bps;
		unsigned char save_bps_frac;

#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif


		// setup pointers
		switch (target) {
			case FOR_RECEIVER:
				trclk = &set->rxclk;
				break;
			case FOR_TRANSMITTER:
				trclk = &set->txclk;
				break;
			default:
				return (GENERAL_ERROR + CODE_ERROR);
				break;
		}

		// this function can only operate when the clock source
		//	of the 'target' is one of the Baud Rate generators
		switch (trclk->clk_tree.A) {
			case CLK_BRG0:
			case CLK_BRG1:
				break;
			default:
				//	not clocked from BRG
				return (GENERAL_ERROR + INVALID_ERROR + BAD_clk_treeA);
				break;
		}

		//	save the current settings, establish new ones
		save_bps = trclk->bps;
		trclk->bps = bps;
		save_bps_frac = trclk->bps_frac;
		trclk->bps_frac = bps_frac;

		// call the setup function
		return setup_brg_ctr (set, (void*)iusc, context, target, trclk->clk_tree.A, 0, resource);

	}

	//**************************************************************************
	//	Set the Line Interface mode for the given port
	//		Args:
	//			port			0 to N-1
	//			line_mode		A setting as defined in file "BHNS_pset.h"
	//			duplex_mode		A setting as defined in file "BHNS_pset.h"
	//			hflow			A setting as defined in file "BHNS_pset.h"
	//		Return:
	//			0	=	success
	//			-1	=	failure
	//
	//	This function can only be called sometime AFTER the above 'iusc_port_setup' function
	//	has been called.
	//
#ifdef QNX4
	int iusc_set_li_mode (int port, void __far *v_iusc, void __far *v_pld, void *context, unsigned char line_mode, unsigned char duplex_mode, unsigned char hflow)
#else
	int iusc_set_li_mode (int port,  void *v_iusc, void *v_pld, void *context, unsigned char line_mode, unsigned char duplex_mode, unsigned char hflow)
#endif

	{
		unsigned short reg1;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif

		if (port >= N_SYNC_PORTS) return (-1);

		reg1 = read_lifc (port, (void*)pld, context);
		reg1 &= ~(LINE_MODE | TERM_ENAB| DUPLEX_MODE | FOUR_TWO_WIRE);
		if ((line_mode & 0x07) > LM_SHUTDOWN) {
			return (-1);
		}
		if (line_mode & LM_TERM) reg1 |= TERM_ENAB;
		else reg1 &= ~TERM_ENAB;
		reg1 = (reg1 & ~LINE_MODE) | SET_LINE_MODE(line_mode);

		// Setup Duplex mode
		switch (duplex_mode) {
			case FOUR_WIRE_RTS:
			case FOUR_WIRE_TXCOMP:
				reg1 |= DUP_FOUR_WIRE;
				// fall into next case
			case TWO_WIRE_RTS:
			case TWO_WIRE_TXCOMP:
				reg1 |= DUP_24;
				break;
			case DUPLEX_FULL:
				break;
			default:
				return (-1);
				break;
		}

		// check if Receiver hardware flow control (using RTS) is enabled,
		//	along with something other than Full-Duplex
		if (hflow & 0x80) {
			if (duplex_mode != DUPLEX_FULL) {
				return (-1);
			}

		}

		if (duplex_mode & DUPLEX_TX_COMP) {
			// setup the IUSC Port-7 pin to be the "TX Complete" signal
			api->write_reg_word((void *)&iusc->PCR.WORD,  (api->read_reg_word((void *)&iusc->PCR.WORD) & ~P7MODE) | P7_TXCOMP);
		}

		write_lifc (port, v_pld, context, reg1);

		return (0);
	}

	//**************************************************************************
	//**************************************************************************
	//	Read the number of bytes/chars available in the Receiver FIFO
	//	Args:
	//		port	0 to N-1
	//	Return:
	//		Number of available RX characters
	//		-1		on error
	//
#ifdef QNX4
	int iusc_get_rx_avail (void __far *v_iusc, void *context)
#else
	int iusc_get_rx_avail (void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		// set the RCSR to allow access to the RX fifo level
		// Page 5-45 says that interrupts need to be disabled here before the next 2 IUSC bus operations
		api->write_reg_word((void *)&iusc->RCSR.WORD,  SEL_RXFIFO_STAT);
		return (READ_INT_REQ_LEVEL(api->read_reg_word((void *)&iusc->RICR.WORD)));
	}


	//**************************************************************************
	//	Read the number of available spaces in the Trasnmitter FIFO
	//	Args:
	//		port	0 to N-1
	//	Return:
	//		Number of available TX spaces
	//		-1		on error
	//
#ifdef QNX4
	int iusc_get_tx_space (void __far *v_iusc, void *context)
#else
	int iusc_get_tx_space (void *v_iusc, void *context)
#endif
	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		// Page 5-45 says that interrupts need to be disabled here before the next 2 IUSC bus operations
		api->write_reg_word((void *)&iusc->TCSR.WORD,  TCSR_ISSUE_TCMD(iusc->TCSR.WORD, api) | SEL_TXFIFO_STAT);
		return (READ_INT_REQ_LEVEL(api->read_reg_word((void *)&iusc->TICR.WORD)));
	}

	//**************************************************************************
	//	Set RTS# (IUSC Port-4 pin) On or Off
	//	Args:
	//		port	0 to N-1
	//		state	A non-zero 'state' value is interpreted as meaning "on"
	//	Return:
	//		none, but exits when port value is wrong
	//
#ifdef QNX4
	void iusc_set_rts (void __far *v_iusc, void *context, int state)
#else
	void iusc_set_rts (void *v_iusc, void *context, int state)
#endif

	{
		unsigned short temp;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		temp = api->read_reg_word((void *)&iusc->PCR.WORD) & ~P7MODE;
		if (state) temp |= P7_LOW;	// low is "on"
		else temp |= P7_HIGH;		// high is "off"
		api->write_reg_word((void *)&iusc->PCR.WORD,  temp);
	}

	//**************************************************************************
	//	Set DTR# (IUSC Port-2 pin) On or Off
	//	Args:
	//		port	0 to N-1
	//		state	A non-zero 'state' value is interpreted as meaning "on"
	//	Return:
	//		none, but exits when port value is wrong
	//
#ifdef QNX4
	void iusc_set_dtr (void __far *v_iusc, void *context, int state)
#else
	void iusc_set_dtr ( void *v_iusc, void *context, int state)
#endif

	{
		unsigned short temp;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		temp = api->read_reg_word((void *)&iusc->PCR.WORD) & ~P2MODE;
		if (state) temp |= P2_LOW;	// low is "on"
		else temp |= P2_HIGH;		// high is "off"
		api->write_reg_word((void *)&iusc->PCR.WORD, temp);
	}

	//**************************************************************************
	//	Get the current state of the CTS pin
	//	Arg:
	//		port	0 to N-1
	//	Return:
	//		0		"Off"
	//		1		"On"
	//		-1		error
	//
#ifdef QNX4
	int iusc_get_cts (void __far *v_iusc, void *context)
#else
	int iusc_get_cts (void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		// clear the latch
		api->write_reg_word((void *)&iusc->MISR.WORD,  CTS_LU);
		if (api->read_reg_word((void *)&iusc->MISR.WORD) & CTS_IS) return (1);	// CTS is "on"
		return (0);		// CTS is "Off"
	}

	//**************************************************************************
	//	Get the current state of the DCD pin
	//	Arg:
	//		port	0 to N-1
	//	Return:
	//		0		"Off"
	//		1		"On"
	//		-1		error
	//
#ifdef QNX4
	int iusc_get_dcd (void __far *v_iusc, void *context)
#else
	int iusc_get_dcd (void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		api->write_reg_word((void *)&iusc->MISR.WORD,  DCD_LU);
		if (api->read_reg_word((void *)&iusc->MISR.WORD) & DCD_IS) return (1);

		return (0);
	}

	//**************************************************************************
	//	Get the current state of DSR (IUSC TXREQ pin)
	//	Arg:
	//		port	0 to N-1
	//	Return:
	//		0		"Off"
	//		1		"On"
	//		-1		error
	//
#ifdef QNX4
	int iusc_get_dsr (void __far *v_iusc, void *context)
#else
	int iusc_get_dsr (void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		api->write_reg_word((void *)&iusc->MISR.WORD, TXR_LU);
		if (api->read_reg_word((void *)&iusc->MISR.WORD) & TXR_IS) return (1);

		return (0);
	}

	//**************************************************************************
	//	Get the current state of RI (IUSC RXREQ pin)
	//	Arg:
	//		port	0 to N-1
	//	Return:
	//		0		"Off"
	//		1		"On"
	//		-1		error
	//
#ifdef QNX4
	int iusc_get_ri (void __far *v_iusc, void *context)
#else
	int iusc_get_ri (void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		api->write_reg_word((void *)&iusc->MISR.WORD,  RXR_LU);
		if (api->read_reg_word((void *)&iusc->MISR.WORD) & RXR_IS) return (1);
		return (0);
	}

	//**************************************************************************
	//	Get a data character from the Receiver
	//		Use of this function is limited to low speed polling operations
	//	Arg:
	//		port	0 to N-1
	//	Return:
	//		character read from RX fifo
	//		0xff		error
	//
#ifdef QNX4
	unsigned char iusc_get_char(void __far *v_iusc, void *context)
#else
	unsigned char iusc_get_char(void *v_iusc, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		while (!(api->read_reg_word((void *)&iusc->RCSR.WORD) & RX_AVAIL));	// wait for a character to arrive in the RX fifo
		return (api->read_reg_char((void *)&iusc->RDR.byte.LSB));
	}

	//**************************************************************************
	//	Check for a data character from the Receiver, and then optionally
	//	assign it to its target location.
	//		Use of this function is limited to low speed polling operations
	//	Args:
	//		port	0 to N-1
	//		*ch		pointer to location to store character
	//		operation
	//			-2		acts like a flushing operation on the receiver fifo
	//			-1		don't check if there is a char available, just get it and return 1
	//			0		check if a char is available, and return 1 if so, otherwise return 0
	//			1		same as "0", except get the character also
	//	Return:
	//		0	character not found
	//		1	character found
	//		-1	error
	//
#ifdef QNX4
	int iusc_check_char(int port, void __far *v_iusc, void *context, unsigned char *ch, int operation)
#else
	int iusc_check_char(int port,  void *v_iusc, void *context, unsigned char *ch, int operation)
#endif
	{
		unsigned char junk;
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif

		if (port >= N_SYNC_PORTS) return (-1);

		switch (operation) {
			case -2:
				while ( api->read_reg_word((void *)&iusc->RCSR.WORD) & RX_AVAIL) {
					junk = api->read_reg_word((void *)&iusc->RDR.byte.LSB);
				}
				return (1);
			case -1:
				*ch = api->read_reg_word((void *)&iusc->RDR.byte.LSB);
				return (1);
			case 0:
			case 1:
				if (api->read_reg_word((void *)&iusc->RCSR.WORD) & RX_AVAIL) {
					if (operation)	*ch = api->read_reg_char((void *)&iusc->RDR.byte.LSB);
					return (1);
				}
				return (0);
			default:
				return (0);
		}
	}

	//**************************************************************************
	//	Put a data charater in the Transmitter
	//		Use of this function is limited to low speed polling operations
	//	Args:
	//		port	0 to N-1
	//		ch		character to store
	//	Return:
	//		none, but exits when port value is wrong
	//
#ifdef QNX4
	void iusc_put_char(int port, void __far *v_iusc, void *context, unsigned char ch)
#else
	void iusc_put_char(int port,  void *v_iusc, void *context, unsigned char ch)
#endif
	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif
		if (port >= N_SYNC_PORTS) return;

		while (!(api->read_reg_char((void *)&iusc->TCSR.byte.LSB) & TX_EMPTY));	//	wait until the TX fifo is empty
		api->write_reg_char((void *)&iusc->TDR.byte.LSB,  ch);
	}



	//**************************************************************************
	//	Setup IUSC Interrupts
	//		args:
	//			iset			Pointer to the setup structure (see file BHNS_isr.h)
	//
	//			intr_mapping	Map the interrupt for the indicated port (iset->port) to
	//							one of 2 Coldfire Interrupts (INT1# or INT2#).
	//							1 = INT1#
	//							2 = INT2#
	//		return:
	//			0		success
	//			-1		failure
	//
	//	!!! Special Note: When using DMA data transfres, the function 'iusc_dma_setup'
	//				MUST be called prior to calling this function.
	//
#ifdef QNX4
	int iusc_interrupt_setup (iusc_int_setup *iset, void __far *v_iusc, void __far *v_pld, void *context)
#else
	int iusc_interrupt_setup (iusc_int_setup *iset, void *v_iusc, void *v_pld, void *context)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
		char port = iset->port;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		volatile _main_pld __far *pld;
		iusc = (_iusc_chip __far *)v_iusc;
		pld = (_main_pld __far *)v_pld;
#else
		volatile _iusc_chip  *iusc;
		volatile _main_pld *pld;
		iusc = (_iusc_chip *)v_iusc;
		pld = (_main_pld *)v_pld;
#endif

		// Clear the MIE bits for the DMA controller
		api->write_reg_word((void *)&iusc->DICR.WORD,  0);// this clears all DMA interrupt enables

		//	Clear the Interrupt IE (interrupt enable) bits, in ICR and DICR
		// this clears MIE, DLC, NV and VIS bits
		api->write_reg_word((void *)&iusc->ICR.WORD,  0);
		api->write_reg_word((void *)&iusc->ICR.WORD, CLEAR_IE | RS_IE | RD_IE | TS_IE | TD_IE | IOP_IE | MISC_IE);	// this clears all Write-Only IE bits

		//	Clear any previous receiver and transmitter status's
		api->write_reg_word((void *)&iusc->RCSR.WORD,  UNLATCH_RCSR_ALL);
		api->write_reg_char((void *)&iusc->TCSR.byte.LSB, UNLATCH_TCSR_ALL);

		// Arm all the interrupts required

		// Receiver DMA Interrupts
		api->write_reg_word((void *)&iusc->RDIAR.WORD,  (api->read_reg_word((void *)&iusc->RDIAR.WORD) & ~DMA_INTR_IA) | iset->rx_dma_ia);
		if (iset->rx_dma_ia) {
			// disable the Receiver data interrupt enable
			api->write_reg_word((void *)&iusc->ICR.WORD, CLEAR_IE | RD_IE);
			//	enable the Receiver DMA data request
			api->write_reg_word((void *)&iusc->DICR.WORD, api->read_reg_word((void *)&iusc->DICR.WORD) | RX_DMA_IE);
		}

		//	Determine if the Receiver data is being transferred via interrrupts
		if (iset->rx_int_level > 0) {
			if (iset->rx_int_level > 32) iset->rx_int_level = 32;
			//	setup the RX interrupt level
			api->write_reg_word((void *)&iusc->RCSR.WORD, SEL_RINT_LEVEL);
			WRITE_RICR_WORD(iusc, SET_INT_REQ_LEVEL(iset->rx_int_level - 1),api);
			api->write_reg_word((void *)&iusc->RCSR.WORD, SEL_RXFIFO_STAT);	// allow RICR[15..8] to read back as the RX FIFO level
			// enable the Receiver data interrupt enable
			api->write_reg_word((void *)&iusc->ICR.WORD, SET_IE | RD_IE);
		}

		//	Setup required Receiver Status IA bits
		iset->rx_status_ia &= RICR_ALL_IA;	//	mask off non-IA bits
		if (iset->rx_status_ia) {
			WRITE_RICR_LSB(iusc, iset->rx_status_ia, api);
			api->write_reg_word((void *)&iusc->ICR.WORD,  SET_IE | RS_IE);
		}

		// this bit is required for operation with a 16-bit bus, WHEN serial data is read as "words"
		//	BUT, I'm not exactly sure how to initially set this bit
		SET_RICR_WORD_STATUS(iusc, WORD_STATUS, api);

		// Transmitter DMA Interrupts
		api->write_reg_word((void *)&iusc->TDIAR.WORD,  (api->read_reg_word((void *)&iusc->TDIAR.WORD) & ~DMA_INTR_IA) | iset->tx_dma_ia);
		if (iset->tx_dma_ia) {
			// disable the Transmitter data interrupt enable
			api->write_reg_word((void *)&iusc->ICR.WORD, CLEAR_IE | TD_IE);
			//	enable the Transmitter DMA data request
			api->write_reg_word((void *)&iusc->DICR.WORD,  api->read_reg_word((void *)&iusc->DICR.WORD) | TX_DMA_IE);
		}

		//	Determine if the Transmitter data is being transferred via interrrupts
		if (iset->tx_int_level > 0) {
			if (iset->tx_int_level > 32) iset->tx_int_level = 32;
			//	setup the TX interrupt level
			api->write_reg_word((void *)&iusc->TCSR.WORD,  TCSR_ISSUE_TCMD(iusc->TCSR.WORD, api) | SEL_TINT_LEVEL);
			api->write_reg_word((void *)&iusc->TICR.WORD,  SET_INT_REQ_LEVEL(iset->tx_int_level - 1));
			// allow TICR[15..8] to read back as the TX FIFO level
			api->write_reg_word((void *)&iusc->TCSR.WORD,  TCSR_ISSUE_TCMD(iusc->TCSR.WORD, api) | SEL_TXFIFO_STAT);
			// enable the Transmitter data interrupt enable
			api->write_reg_word((void *)&iusc->ICR.WORD,  SET_IE | TD_IE);
		}

		//	Setup required Transmitter Status IA bits
		iset->tx_status_ia &= TICR_ALL_IA;		// mask off non-IA bits
		if (iset->tx_status_ia) {
			api->write_reg_char((void *)&iusc->TICR.byte.LSB,  iset->tx_status_ia);
			api->write_reg_word((void *)&iusc->ICR.WORD,   SET_IE | TS_IE);
		}

		// Setup required I/O pin and Misc IA bits
		if (iset->io_misc_ia) {
			// don't allow the RxC and TxC or BRG interrupts
			api->write_reg_word((void *)&iusc->SICR.WORD, iset->io_misc_ia & ~(TRXC_IA | BRG_IA));
			api->write_reg_word((void *)&iusc->ICR.WORD,  SET_IE | (IOP_IE | MISC_IE));
			// clear any latched misc LU bits
			api->write_reg_word((void *)&iusc->MISR.WORD,  ALL_LU);
		}

		//	Setup vectors and enable VIS
		api->write_reg_word((void *)&iusc->ICR.WORD,  (api->read_reg_word((void *)&iusc->ICR.WORD) & ~(IE_OP | VIS)) | STAT_ALWAYS);// place the port number into the upper 4 bits
		api->write_reg_word((void *)&iusc->IVR.WORD,  SERIAL_VECTOR | ((port & 3) << 4));// place the port number into the upper 4 bits
		api->write_reg_word((void *)&iusc->DICR.WORD,  api->read_reg_word((void *)&iusc->DICR.WORD)| DMA_VIS);
		api->write_reg_word((void *)&iusc->DIVR.WORD,  DMA_VECTOR | ((port & 3) << 4));// place the port number into the upper 4 bits;

		//	clean up any spurious IP and IUS bits
		if (api->read_reg_word((void *)&iusc->DCCR.WORD) > 0) {
			api->write_reg_word((void *)&iusc->DCCR.WORD, CLEAR_IP_IUS | ALL_DCCR_IP);
		}
		if (api->read_reg_word((void *)&iusc->SDIR.WORD)) {
			api->write_reg_word((void *)&iusc->CDIR.WORD,  CLR_ALL_CDIR);
		}


		return (0);
	}

	//**************************************************************************
	//	Fake a Serial Interrupt
	//		Useful for testing purposes
	//		The above interrupt setup function MUST be called prior to calling this function
	//
	//	Args:
	//		port		Port number (0 to N-1)
	//
	//		ip_mask		A value defining which IP bit(s) to set
	//					(see IP bits in the DCCR register, in file BHNS_iusc.h)
	//
#ifdef QNX4
	void iusc_fake_serial_int (void __far *v_iusc, void *context, unsigned char ip_mask)
#else
	void iusc_fake_serial_int (void *v_iusc, void *context, unsigned char ip_mask)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		// Fake the interrupt by setting the IP bit(s) in the DCCR register
		api->write_reg_char((void *)&iusc->DCCR.byte.LSB,  SET_IP | (ip_mask & 0x3F));
	}

#ifdef QNX4
	void iusc_clear_fake_serial_int (void __far *v_iusc, void *context, unsigned char ip_mask)
#else
	void iusc_clear_fake_serial_int (void *v_iusc, void *context, unsigned char ip_mask)
#endif
	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif

		// Clear the Fake the interrupt by clearing the IP bit(s) in the DCCR register
		api->write_reg_char((void *)&iusc->DCCR.byte.LSB,  CLEAR_IP | (ip_mask & 0x3F));
	}

	//**************************************************************************
	//	Setup the loopback setting
	//
	//	Args:
	//		port		Port number (0 to N-1)
	//
	//		rt_mode		The loopback mode setting
	//					(use the #defines for the RTMODE field of the CCAR register (in file BHNS_iusc.h)
	//
#ifdef QNX4
	int iusc_setup_loopback ( void __far *v_iusc, void *context, unsigned short rt_mode)
#else
	int iusc_setup_loopback ( void *v_iusc, void *context, unsigned short rt_mode)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		SETUP_LOOPBACK(iusc, rt_mode, api);
		return (0);
	}

	//**************************************************************************
	//	Support items for BREAK function
	#define MBAR_BASE	0x10000000		// most likely location for the MBAR
	typedef struct {
		unsigned short tmr;
		unsigned short :16;
		unsigned short trr;
		unsigned short :16;
		unsigned short tcap;
		unsigned short :16;
		unsigned short tcn;
		unsigned short :16;
		unsigned short ter;
	} _timer;

	//**************************************************************************
	//	Transmit a BREAK
	//	(Mainly useful in Async mode)
	//
	//	Args:
	//		port		Port number (0 to N-1)
	//
	//		state_duration
	//				0=		turn BREAK Off
	//				1=		turn BREAK On
	//				>1		turn BREAK On then Off, for a duration specified by the
	//						value of 'state_duration' in nS (nano-seconds)
	//				-1=		turn BREAK On then Off, for a duration determined by the bps setting
	///						of the port (12 bit times)
	//
#ifdef QNX4
	int iusc_send_break( void __far *v_iusc, void *context, long state)
#else
	int iusc_send_break( void *v_iusc, void *context, long state)
#endif

	{
		struct _api_context *api = (struct _api_context *)context;
#ifdef QNX4
		volatile _iusc_chip __far *iusc;
		iusc = (_iusc_chip __far *)v_iusc;
#else
		volatile _iusc_chip  *iusc;
		iusc = (_iusc_chip *)v_iusc;
#endif


		if (state) {
			//	enable BREAK
			api->write_reg_word((void *)&iusc->IOCR.WORD,  (api->read_reg_word((void *)&iusc->IOCR.WORD) & ~TXD_MODE) | TXD_LOW);
		}
		else {
			//	disable BREAK
			api->write_reg_word((void *)&iusc->IOCR.WORD,  (api->read_reg_word((void *)&iusc->IOCR.WORD) & ~TXD_MODE) | TXD_TXD);
		}

		return (0);
	}




