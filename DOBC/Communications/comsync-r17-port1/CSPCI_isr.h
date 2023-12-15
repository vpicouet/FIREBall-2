#ifndef _BHNS_isr_h
	#define _BHNS_isr_h

	/****************************************************************************************/
	//	Interrupt Vector (An 8-bit value obtained by performing the Interrupt Acknowledge bus cycle)
	#define DMA_VECTOR		0x00		//	bit0=0 will indicate a DMA interrupt
	#define SERIAL_VECTOR	0x01		//	bit0=1 will indicate a Serial interrupt
	//		Value for the "TypeCode" field of the respective vector
	#define GET_SER_TYPE(vector)	(((vector) & 0x0E) >> 1)
		#define SER_NO_PEND			0
		#define SER_MISC_PEND		1
		#define SER_IO_PEND			2
		#define SER_TX_DATA_PEND	3
		#define SER_TX_STAT_PEND	4
		#define SER_RX_DATA_PEND	5
		#define SER_RX_STAT_PEND	6
	#define GET_DMA_TYPE(vector)	(((vector) & 0x06) >> 1)
		#define DMA_NO_PEND			0
		#define DMA_TX_EVENT		2
		#define DMA_RX_EVENT		3

	//=============================================================================================
	// Data type defining how to setup the Interrupt mechanisms of the IUSC
	//
	typedef struct {
		//	this is here for convenience (0 -> N-1)
		char port;

		//	Use the #defines for the IA bits in the RICR register (in file CSPCI_iusc.h)
		unsigned char rx_status_ia;

		//	Number of FULL positions in the RX FIFO which cause an interrupt (1 to 32)
		//	Use a value of 0 (zero) to indicate that the RX data interrupt is disabled
		unsigned char rx_int_level;

		//	Use the #defines for the IA bits in the TICR register (in file CSPCI_iusc.h)
		unsigned char tx_status_ia;

		//	Number of EMPTY positions in the TX FIFO which cause an interrupt (1 to 32)
		//	Use a value of 0 (zero) to indicate that the TX data interrupt is disabled
		unsigned char tx_int_level;

		//	Use the #defines for the IA bits in the SICR register (in file CSPCI_iusc.h)
		unsigned short io_misc_ia;

		//	Use the #defines for the IA bits in the RDIAR register (in file CSPCI_iusc.h)
		unsigned char rx_dma_ia;

		//	Use the #defines for the IA bits in the TDIAR register (in file CSPCI_iusc.h)
		unsigned char tx_dma_ia;
	} iusc_int_setup;

	/****************************************************************************************/
	//	Reasonably good sets of default settings
	//
	#define BHNS_ISR_SETTINGS_FOR_DMA(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,					\
			/* rx_int_level */	\
			0,					\
			/* tx_status_ia */	\
			0,					\
			/* tx_int_level */	\
			0,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			EOA_EOL_IA,			\
			/* tx_dma_ia */		\
			EOA_EOL_IA,			\
		}
	
	#define BHNS_ISR_SETTINGS_TYPICAL(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,					\
			/* rx_int_level */	\
			24,					\
			/* tx_status_ia */	\
			0,					\
			/* tx_int_level */	\
			24,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}

	//=====================================================
	//	Used with the SIMPLE_MONOSYNC port setup
	//		The RX status interrupt IDLE_RECV_IA could be used to
	//		place the Receiver back into Hunt.
	//
	#define ISR_SETTINGS_FOR_SIMPLE_MONOSYNC(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			EXITED_HUNT_IA,		\
			/* rx_int_level */	\
			16,					\
			/* tx_status_ia */	\
			TX_UNDER_IA,		\
			/* tx_int_level */	\
			24,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}

	//=====================================================
	//	An empty settings setup (all interrupt sources disabled)
	//
	#define ISR_SETTINGS_NONE(pnum)		\
		{	pnum,				\
			/* rx_status_ia */	\
			0,		\
			/* rx_int_level */	\
			0,					\
			/* tx_status_ia */	\
			0,		\
			/* tx_int_level */	\
			0,					\
			/* io_misc_ia */	\
			0,					\
			/* rx_dma_ia */		\
			0,					\
			/* tx_dma_ia */		\
			0,					\
		}

#endif
