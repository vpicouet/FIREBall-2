/*******************************************************************************
**	File:			drv_struct.h
**	Author:			Hasan Jamal
**
**
**	Description:
**		Linux Comsync driver structures declaration
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

#ifndef _comsync_drv_struct_h
	#define _comsync_drv_struct_h

#include <linux/wait.h>

#define MAX_COMSYNC_BOARD	8
#define NUM_COMSYNC_PORT	2

/******************************************************************************
* Circular Buffer Management
******************************************************************************/

#define CIRC_BUF_DATA_AVAIL(head, tail, size) ((size +  head - tail) % size)
#define CIRC_BUF_EMPTY_AVAIL(head, tail, size)  ((size) - CIRC_BUF_DATA_AVAIL((head),(tail),(size)) - 1)

#define EVENT_NONE            0x00000000
#define EVENT_ALL             0xFFFFFFFF
/* enhanced interrupt event sources */
#define EVENT_IDLE_RECVD      0x00000001
#define EVENT_ABORT           0x00000002  /* HDLC/SDLC Abort or Go Ahead sequence */
#define EVENT_RCC_UNDER       0x00000004
#define EVENT_PRE_SENT        0x00000008
#define EVENT_IDLE_SENT       0x00000010
#define EVENT_ABORT_SENT      0x00000020
#define EVENT_EOFM_SENT       0x00000040
#define EVENT_CRC_SENT        0x00000080
#define EVENT_TX_UNDER        0x00000100
#define EVENT_RI_FALL         0x00000200
#define EVENT_RI_RISE         0x00000400
#define EVENT_DCD_FALL        0x00000800
#define EVENT_DCD_RISE        0x00001000
#define EVENT_CTS_FALL        0x00002000
#define EVENT_CTS_RISE        0x00004000
#define EVENT_DSR_FALL        0x00008000
#define EVENT_DSR_RISE        0x00010000
#define EVENT_DPLL_DSYNC      0x00020000
#define EVENT_CRC             0x00040000
#define EVENT_SHORT_FR_CV     0x00100000
#define EVENT_OE       		  0x00200000
#define EVENT_PE              0x00400000
#define EVENT_FE              0x00800000
#define EVENT_BREAK           0x01000000

/* enhanced buffer event sources */
#define EVENT_RX_BUF_LOW      0x02000000
#define EVENT_TX_BUF_LOW      0x04000000
#define EVENT_RX_BUF_OVERRUN  0x08000000




// port open conditions values
#define OK   0
#define NOT_PRESENT 0
#define DEV_READY   1
#define DEV_IN_USE  2

#define LSET_PARITY_NONE   'n'
#define LSET_PARITY_EVEN   'e'
#define LSET_PARITY_ODD    'o'
#define LSET_PARITY_MARK   '1'
#define LSET_PARITY_SPACE  '0'

#define LSET_SFLOW_NONE  'n'
#define LSET_SFLOW_RX    'r'
#define LSET_SFLOW_TX    't'
#define LSET_SFLOW_RXTX  'b'

#define LSET_HFLOW_NONE        0x00
#define LSET_HFLOW_RTS_TOGGLE  0x01
#define LSET_HFLOW_DTR         0x02
#define LSET_HFLOW_CTS         0x08
#define LSET_HFLOW_DSR         0x10
#define LSET_HFLOW_RTS         0x80
#define LSET_HFLOW_DCD_RX_CTS_TX      0x20 // ESCC /DCD becomes receiver enable /CTS becomes transmitter enable


// driver internal buffer size
extern const int BUF_SIZE;// 		=	8192; //8K
#define DMA_BUF_REQEST 16 * 1024 //16k

#define	DMA_READ_SIZE 		BUF_SIZE/4
// threshold level for receive buffer to decide flow control action
#define RX_BUF_THRESHOLD BUF_SIZE/4

#define NUM_DMA_LIST	2

struct escc_struct {
	// ESCC write registers
	 _scc WriteReg;
	// ESCC read registers
	 _scc ReadReg;
	// base address resister
	char *base_address;
	// ESCC port registers
	char *reg_address;
	// bitrate
	unsigned long current_baud;
	// flag to indicate sync character hunt active
	unsigned char hunt_mode;
	// port index
	char index;
	// flag to indicate write dma enabled
	char use_dma_write;
	// flag to indicate read dma enabled
	char use_dma_read;
	// flag to indicate write dma busy
	char write_dma_busy;
	// flag to indicate read dma enabled
	char read_dma_busy;
	// read dma channel number
	char read_dma_ch;
	// write dma channel number
	char write_dma_ch;
	// current read dma size
	int dma_read_size;
	// flag to indicate whether the board is ComsyncIII
	unsigned char flag_comsync_III;
};

struct _sdlc_frame_info {
	unsigned short len;
	unsigned char stat;
};
struct _sdlc_frames {
	struct _sdlc_frame_info *info;
	// circular buffer management head/tail
	unsigned head;
	unsigned tail;
};

// all receive info is here  for any port
struct rx_struct {

	// total byte count for diagnostic purpose
	unsigned long count;
	// last character found time
	unsigned long last_char_strobe;
	// last rx_avail in timer routine
	unsigned int timer_rx_avail;
	// this is updated in timer routine to decide to toggle rx int for stuck characters
	unsigned long timer_last_char_strobe;
	// flag to hold reception due to flow control
	unsigned int holding;
	// data buffer
	unsigned char *buffer;
	// Comsync_III DMA data buffer
	unsigned char *dma_buffer[NUM_DMA_LIST+1];
	// circular buffer management head/tail
	unsigned head;
	unsigned tail;
	// fifo address
	unsigned char *fifo;

	/* Wait queue for available space. */
	wait_queue_head_t  wait;
	// flag for wait queue
	int wakeup;
	// Comsync_III flag for dma buffer index
	int dma_buf_index;
	// phisical address to program
	unsigned long dma_phy_addr[NUM_DMA_LIST+1];
	// DMA list address
	void __iomem *dma_list_addr[NUM_DMA_LIST];
	// DMA count address
	void __iomem *dma_count_addr[NUM_DMA_LIST];
	// DMA count programmed
	unsigned long  dma_count;
	// DMA index to current buffer pointer
	unsigned long  dma_buf_cur_index;
	
	struct _sdlc_frames sdlc_frames;
	
};

// all transmit info is here for any port
struct tx_struct
{
	// buffer length to transmit
	unsigned long  len;
	// data buffer
	unsigned char *buffer;
	// current data buffer slot
	unsigned char *cur_buf_slot;
	// fifo address
	unsigned char *fifo;

	struct {
		/* Flag to send xon. */
		int  send_xon;
		/* Flag to send xoff. */
		int  send_xoff;
		/* Flag to hold transmission due to flow control. */
		unsigned int  sholding;
		/* Flag to kick start transmission stopped due to flow control. */
		int  restart;
		/* Flag to indicate RTS/CTS flow. */
		int  rts;
		/* Flag to indicate DTR/DSR flow. */
		int  dtr;
	} flow;

	// total byte count for diagnostic purpose
	unsigned long  count;
	/* Wait queue for available space. */
	wait_queue_head_t  wait;
	// flag for wait queue
	int wakeup;
	// Comsync_III DMA data buffer
	unsigned char *dma_buffer[NUM_DMA_LIST+1];
	// Comsync_III flag for dma driver buffer index
	int dma_addr_buf_index;
	// Comsync_III flag for dma FPGA buffer index
	int dma_buf_index;
	// phisical address to program
	unsigned long dma_phy_addr[NUM_DMA_LIST+1];
	// DMA list address
	void __iomem *dma_list_addr[NUM_DMA_LIST];
	// DMA count address
	void __iomem *dma_count_addr[NUM_DMA_LIST];
	int tx_int_wait;
	
};


// this is the port driver structure for individual port
struct sync_port_drv_struct
{
	// index of this port
	unsigned short index;
	// currently holds the open\close status but can have more status info
	int status_device;
	// receive data structure
	struct rx_struct rx;
	// transmit data structure
	struct tx_struct tx;
	// iusc registers
	volatile _iusc_chip *iusc;
	volatile _main_pld *pld;
	// approximate tick time for FIFO size w.r.t bps
	unsigned long fifo_ticks;
	// port settings
	BHN_PortSettings port_settings;
	// I/O access api
	struct _api_context api;
	// events mask for various events
	unsigned long events;
	// spinlock to synchonize threads
	spinlock_t              driver_lock;

	// non block write flag
	unsigned char non_block_io;
	// flag to indicate whether board has ESCC
	unsigned char flag_escc;
	// ESCC register structure
	struct escc_struct escc;
};



struct bus_driver_struct {
	// PCI BAR0 start address
	unsigned long bar0_start;
	// PCI BAR0 len
	unsigned long bar0_len;
	// PCI BAR1 start address
	unsigned long bar1_start;
	// PCI BAR1 len
	unsigned long bar1_len;
	// PCI BAR2 start address
	unsigned long bar2_start;
	// PCI BAR2 len
	unsigned long bar2_len;
	// IRQ number
	unsigned int comsync_irq;
	// returned structure from pci_get_subsys API
	struct pci_dev *pci_dev_struct;
	// IUSC pointer
	void *b_iusc;
	// PLD pointer
	void *b_pld;
	// PLX pointer
	_PLX_registers  *PLXAperture;
	// port driver's array
	struct sync_port_drv_struct sync_port_driver[NUM_COMSYNC_PORT];
	// I/O access api
	struct _api_context api;
	// base address register
	void __iomem *escc_base_address;
	// flag to indicate MIE enabled
	char flag_escc_mie;
	
	void __iomem *comsyncIII_bar1;
	void __iomem *comsyncIII_bar2;
};


#endif
