/************************************************************
**	File:	test.c
**	Author:
**			Hasan Jamal
**
**	Description:
**		Sample application for various tests such as set/clear line signals, loopback, purge etc.
**
**	Revision history:
**
**	Copyright (c) 2009, CTI, Connect Tech Inc. All Rights Reserved.
**
**	THIS IS THE UNPUBLISHED PROPRIETARY SOURCE CODE OF CONNECT TECH INC.
**	The copyright notice above does not evidence any actual or intended
**	publication of such source code.
**
**	This module contains Proprietary Information of Connect Tech, Inc
**	and should be treated as Confidential.
************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>

#include "CSPCI_pset.h"
#include "ioctls.h"

static char *version = "Comsync test util version 0.03 March 25 2013\nConnect Tech Inc.\n\n";

#define FALSE 0
#define TRUE 1

#define MAX_CMD   20 //not a real number

#define RAW_MODE		1
#define FILE_MODE		2
#define LOOPBACK		3
#define COMMAND			4

volatile int goodPacket = 0;
int dataMismatch;
volatile unsigned long txPackets = 0;


// total bytes recevd
unsigned long recv_byte[2] = {0,0};
// total bytes sent
unsigned long send_byte[2] = {0,0};


#define RX	1
#define TX	2
#define TX_RX	3

#define CMD_USE "Command Usage: \n\
1 Set RTS\n\
2 Clear RTS\n\
3 Set DTR\n\
4 Clear DTR\n\
5 Set Break\n\
6 Clear Break\n\
7 Purge Rx\n\
8 Purge Tx\n\
9 Purge RxTx\n\
10 Get line state \n\
11 Transmit\n\
12 Receive\n\
13 loopback\n\
14 exit\n"



// buffer size
#define MAX_BUF_SIZE  4096
unsigned char  pakBuf[MAX_BUF_SIZE];

#define ESC	0x1b

struct _mode
{
	// mode for tansmit
	int tx;
	// mode for receive
	int rx;
};

struct _port_args {
	struct _mode mode;
	// packet size for receive and transmit
	int packetSize;
	// delay in ms before a read
	long delayRx;
	// delay in ms before a transmit
	long delayTx;
	// port index
	int port;
	// file descriptor for the port
	int fd;
	// file pointer for transmit/receive data
	FILE *fp;
	// flag to indicate send the file once or repeat
	char sendOnce;
	// flag to indicate whether non-blocking read/write to perform
	int non_block_io;
	// flag to indicate to use select api to wait for data
	int use_select;
	// flag to indicate to print events
	int show_events;
	// flag to indicate to exit thread
	int exit;
};

// port configuration struct
	BHN_PortSettings pset1={0};
	BHN_PortSettings pset2={0};

//various identifier list in comsync.cfg
static char *IdentifierList[] = {
		"port",
		"smode_tx",
		"smode_rx",
		"rxclk_ref_freq",
		"rxclk_bps_error",
		"rxclk_bps",
		"rxclk_bps_frac",
		"rxclk_async_div",
		"rxclk_dpll_div",
		"rxclk_ctr_div",
		"rxclk_enc_dec",
		"rxclk_ct_levelA",
		"rxclk_ct_levelB",
		"rxclk_ct_levelC",
		"rxclk_ct_levelD",
		"rxclk_clk_pin",
		"txclk_ref_freq",
		"txclk_bps_error",
		"txclk_bps",
		"txclk_bps_frac",
		"txclk_async_div",
		"txclk_dpll_div",
		"txclk_ctr_div",
		"txclk_enc_dec",
		"txclk_ct_levelA",
		"txclk_ct_levelB",
		"txclk_ct_levelC",
		"txclk_ct_levelD",
		"txclk_clk_pin",
		"pre_pat",
		"pre_len",
		"tx_idle",
		"line_mode",
		"duplex_mode",
		"tx0",
		"tx1",
		"tx_len",
		"rx0",
		"rx1",
		"rx_len",
		"strip_sync",
		"dbits_tx",
		"dbits_rx",
		"tx_frac_stop",
		"parity_tx",
		"parity_rx",
		"tx_type",
		"tx_start",
		"tx_mode",
		"rx_type",
		"rx_start",
		"sflow",
		"xoff",
		"xon",
		"hflow",
		"tx_flag_share_zero",
		"rx_addr_control",
		"tx_underrun",
		"disable_dma_read",
		"disable_dma_write",
		"lloop",
		"last_char_crc_stat"
	};

#define NUM(s) sizeof(s) / sizeof(*s)
#define SYNC_PARAMS NUM(IdentifierList)

// character find from string
int  strchfind(char c, char *str)
{
	int i;

	for (i = 0; str[i] != 0; i++) if (str[i] == c) return i;
	return -1;
}

// parsing comsync.cfg file
int  ParseCfgFile(char *s1, char *s2, BHN_PortSettings *pset)
{
	int i, len;


	//removing extra space or enter from end
	len = strlen(s1);
	for(i=0; i<len; i++)
	{
		if ((s1[i] == 0xa) || (s1[i] == 0x20))
			s1[i] = 0;
	}
	len = strlen(s2);
	for(i=0; i<len; i++)
	{
		if ((s2[i] == 0xa) || (s2[i] == 0x20))
			s2[i] = 0;
	}

	for (i = 0; i < SYNC_PARAMS; i++)
		if(strlen(s1) ==  strlen(IdentifierList[i]))
			if (strncmp(s1, IdentifierList[i], strlen(IdentifierList[i])) == 0)
				break;


	switch (i)
	{
	case 0:
		pset->port = atoi(s2);
		return TRUE;

	case 1:
		pset->smode.tx = atoi(s2);
		return TRUE;

	case 2:
		pset->smode.rx = atoi(s2);
		return TRUE;

	case 3:
		pset->rxclk.ref_freq = atol(s2);
		return TRUE;

	case 4:
		pset->rxclk.bps_error = atol(s2);
		return TRUE;

	case 5:
		pset->rxclk.bps = atol(s2);
		return TRUE;

	case 6:
		pset->rxclk.bps_frac = atoi(s2);
		return TRUE;

	case 7:
		pset->rxclk.async_div = atoi(s2);
		return TRUE;

	case 8:
		pset->rxclk.dpll_div = atoi(s2);
		return TRUE;

	case 9:
		pset->rxclk.ctr_div = atoi(s2);
		return TRUE;

	case 10:
		pset->rxclk.enc_dec = atoi(s2);
		return TRUE;

	case 11:
		pset->rxclk.clk_tree.A = atoi(s2);
		return TRUE;

	case 12:
		pset->rxclk.clk_tree.B = atoi(s2);
		return TRUE;

	case 13:
		pset->rxclk.clk_tree.C = atoi(s2);
		return TRUE;

	case 14:
		pset->rxclk.clk_tree.D = atoi(s2);
		return TRUE;

	case 15:
		pset->rxclk.clk_pin = atoi(s2);
		return TRUE;

	case 16:
		pset->txclk.ref_freq = atol(s2);
		return TRUE;

	case 17:
		pset->txclk.bps_error = atol(s2);
		return TRUE;

	case 18:
		pset->txclk.bps = atol(s2);
		return TRUE;

	case 19:
		pset->txclk.bps_frac = atoi(s2);
		return TRUE;

	case 20:
		pset->txclk.async_div = atoi(s2);
		return TRUE;

	case 21:
		pset->txclk.dpll_div = atoi(s2);
		return TRUE;

	case 22:
		pset->txclk.ctr_div = atoi(s2);
		return TRUE;

	case 23:
		pset->txclk.enc_dec = atoi(s2);
		return TRUE;

	case 24:
		pset->txclk.clk_tree.A = atoi(s2);
		return TRUE;

	case 25:
		pset->txclk.clk_tree.B = atoi(s2);
		return TRUE;

	case 26:
		pset->txclk.clk_tree.C = atoi(s2);
		return TRUE;

	case 27:
		pset->txclk.clk_tree.D = atoi(s2);
		return TRUE;

	case 28:
		pset->txclk.clk_pin = atoi(s2);
		return TRUE;

	case 29:
		pset->tx_pre_idle.pre_pat = atoi(s2);
		return TRUE;

	case 30:
		pset->tx_pre_idle.pre_len = atoi(s2);
		return TRUE;

	case 31:
		pset->tx_pre_idle.tx_idle = atoi(s2);
		return TRUE;

	case 32:
		pset->line_mode = atoi(s2);
		return TRUE;

	case 33:
		pset->duplex_mode = atoi(s2);
		return TRUE;

	case 34:
		pset->sync_addr.tx0 = atoi(s2);
		return TRUE;

	case 35:
		pset->sync_addr.tx1 = atoi(s2);
		return TRUE;

	case 36:
		pset->sync_addr.tx_len = atoi(s2);
		return TRUE;

	case 37:
		pset->sync_addr.rx0 = atoi(s2);
		return TRUE;

	case 38:
		pset->sync_addr.rx1 = atoi(s2);
		return TRUE;

	case 39:
		pset->sync_addr.rx_len = atoi(s2);
		return TRUE;

	case 40:
		pset->sync_addr.strip_sync = atoi(s2);
		return TRUE;

	case 41:
		pset->dbits.tx = atoi(s2);
		return TRUE;

	case 42:
		pset->dbits.rx = atoi(s2);
		return TRUE;

	case 43:
		pset->tx_frac_stop = atoi(s2);
		return TRUE;

	case 44:
		pset->parity.tx = atoi(s2);
		return TRUE;

	case 45:
		pset->parity.rx = atoi(s2);
		return TRUE;

	case 46:
		pset->crc.tx_type = atoi(s2);
		return TRUE;

	case 47:
		pset->crc.tx_start = atoi(s2);
		return TRUE;

	case 48:
		pset->crc.tx_mode = atoi(s2);
		return TRUE;

	case 49:
		pset->crc.rx_type = atoi(s2);
		return TRUE;

	case 50:
		pset->crc.rx_start = atoi(s2);
		return TRUE;

	case 51:
		pset->async_settings.sflow = atoi(s2);
		return TRUE;

	case 52:
		pset->async_settings.xoff = atoi(s2);
		return TRUE;

	case 53:
		pset->async_settings.xon = atoi(s2);
		return TRUE;

	case 54:
		pset->async_settings.hflow = atoi(s2);
		return TRUE;

	case 55:
		pset->hdlc.tx_flag_share_zero = atoi(s2);
		return TRUE;

	case 56:
		pset->hdlc.rx_addr_control = atoi(s2);
		return TRUE;

	case 57:
		pset->hdlc.tx_underrun = atoi(s2);
		return TRUE;

	case 58:
		pset->disable_dma_read = atoi(s2);
		return TRUE;

	case 59:
		pset->disable_dma_write = atoi(s2);
		return TRUE;

	case 60:
		pset->lloop = atoi(s2);
		return TRUE;
		
	case 61:
		pset->last_char_crc_stat  = atoi(s2);
		return TRUE;
		
	default:
		break;
	}

	return FALSE;
}

// read parameters from comsync.cfg
int  ReadCfgFile(char *filename, BHN_PortSettings *cfg)
{
	FILE *fp;
	char str[100];
	int  i, len, pos1, pos2, ret, lineNum = 0;
	int  param = 0;


	fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		printf("%s file not found\n", filename);

		return -1;
	}
	/* parse config file */
	for ( ; ; )
	{
		lineNum++;

		/* read line by line */
		if (fgets(str, 99, fp) == NULL)
		{
			if (feof(fp)) break;
			fclose(fp);
			return -1;
		}

		len = strlen(str);
		pos2 = strchfind('=', str);
		if ((pos2 > 0) && (pos2 < (len-1)))
		{
			/* '=' was found, so intepret the line of text */
			str[pos2] = 0;
			//remove extra space from beginning
			while(pos2 < (len-1))
			{
				if (str[pos2+1] == 0x20)
					pos2++;
				else
					break;
			}
			pos1=0;
			for(i=0;i<pos2;i++)
			{
				if(str[i] == 0x20)
					pos1++;
				else
					break;
			}
			ret = ParseCfgFile(&str[pos1], &str[pos2+1], cfg);
			if (ret)
				param++;
		}
	}
	fclose(fp);

	return param;
}

// parameter input for receive, transmit or loopback
void GetDataOpParams(struct _port_args *Params, int mode)
{
	int val;
	// file name for transmit data
	char filename[100];
	char c;

	while(1)
	{
		printf("enter packet size (Max 4096)\n");
		scanf("%d",&val);
		if(val <= 0 || val > 4096)
		{
			printf("wrong values\n");
			continue;
		}
		break;
	}
	Params->packetSize = val;

	if(Params->delayRx != -1)
	{
		while(1)
		{
			printf("enter delay between packets for receive (milliseconds)\n");
			scanf("%d",&val);
			if(val < 0)
			{
				printf("wrong values %d\n",val);
				continue;
			}
			break;
		}
		Params->delayRx = val * 1000;
	}
	if(Params->delayTx != -1)
	{
		while(1)
		{
			printf("enter delay between packets for transmit (milliseconds)\n");
			scanf("%d",&val);
			if(val < 0)
			{
				printf("wrong values %d\n",val);
				continue;
			}
			break;
		}
		Params->delayTx = val * 1000;
	}

	if(mode == TX_RX)
	{
		Params->mode.tx = LOOPBACK;
		Params->mode.rx = LOOPBACK;
	}
	else if(mode == RX)
	{
		while(1)
		{
			printf("enter data receive mode raw:1, file:2\n");
			scanf("%d",&val);
			if(val > 2 || val < 1)
			{
				printf("wrong values\n");
				continue;
			}
			break;
		}
		if(val == FILE_MODE)
		{
			while(1)
			{
				printf("enter file name\n");
				scanf("%s",filename);
				Params->fp = fopen(filename,"wt");
				if (!Params->fp)
				{
					printf("invalid filename\n");
				}
				else
				{
					break;
				}
			}
		}

		Params->mode.rx = val;

	}
	else if(mode == TX)
	{
		while(1)
		{
			printf("enter data transmit mode raw:1, file:2\n");
			scanf("%d",&val);
			if(val > 2 || val < 1)
			{
				printf("wrong values\n");
				continue;
			}
			break;
		}

		if(val == FILE_MODE)
		{
			while(1)
			{
				printf("enter file name\n");
				scanf("%s",filename);
				Params->fp = fopen(filename,"rt");
				if (!Params->fp)
				{
					printf("invalid filename\n");
					continue;
				}
				else
				{
					break;
				}
			}

			while(1)
			{
				printf("Send once? y/n\n");
				scanf("%c",&Params->sendOnce);
				if(Params->sendOnce == 'y' || Params->sendOnce == 'n')
					break;
			}
		}

		Params->mode.tx = val;
	}

}

// receive thread for each port
void *thread_read(void *arg) {
  int i,j, len, ret;
  // data buffer
  unsigned char pBuf[MAX_BUF_SIZE];
   struct _port_args *port_args = arg;
  // old running mode
  int oldMode;
  // current number bytes
  int curNumbytes = 0;
  // flag to indicate packet receive status
  unsigned char pakRecv = FALSE;
  // received raw data input buffer
  unsigned char inBuf[MAX_BUF_SIZE];
  // current position of receive raw data buffer
  int curPos = 0;
  // vars to use with select api
  fd_set tmpfds;
  fd_set readfds;
  int fdwidth;
  // events for TIOGETEVENT ioctl
  unsigned long events=0;
  int packetSize;

  FD_ZERO(&readfds);
  FD_SET(port_args->fd, &readfds);
  fdwidth=1;
  fdwidth+=port_args->fd;

  Begin:
	while(port_args->mode.rx < 0 )
	{
		usleep(1000);
		if (port_args->exit)
			return;
	}

	oldMode = port_args->mode.rx;
	
	packetSize = port_args->packetSize;
	if(port_args->mode.rx == LOOPBACK)
		if(pset1.crc.tx_type)
			packetSize +=2;	// receiver will receive CRC if the board has ESCC
	if(pset1.last_char_crc_stat)
		packetSize++;
		
  while(1)
	{
		if(oldMode != port_args->mode.rx)
			goto Begin;

		if(port_args->use_select > 0)
		{
			tmpfds = readfds;
			i = select(fdwidth, &tmpfds, NULL, NULL, NULL);
			if(i <= 0)
				break;
		}
		len = read(port_args->fd, pBuf, packetSize);
		if(len < 0)
		{
			printf("read failed errno %d\n",errno);
			exit(1);
		}
		
		if(len == 0)
			continue;
		curNumbytes +=len;
		if(curNumbytes >= port_args->packetSize)
			pakRecv = TRUE;
		recv_byte[port_args->port] += len;

		if(port_args->show_events > 0)
		{
			ret = ioctl(port_args->fd, TIOGETEVENT, &events);
			if (ret != 0) {
				printf("TIOGETEVENT failed ret %x\n", ret);
				abort();
			}
			if(events)
			{
				printf("events %x\n",events);
				events = 0;
			}
		}
		
		if(pset1.last_char_crc_stat && pset1.smode.rx == SMODE_HDLC )
			if(pBuf[packetSize-1] & FRAME_CRC_ERROR)
			{
				printf("CRC fail\n");
			}
			
		// print the raw data
		for(i=0;i<len;i++)
		{
			if(pset1.smode.rx == SMODE_MONO || pset1.smode.rx == SMODE_BISYNC)
			{
				if((pBuf[i] == 0xff) || (pBuf[i] == 0x16))
				{
					//printf("TIOHUNT \n");
					ret = ioctl(port_args->fd, TIOHUNT);
					if (ret != 0) {
						printf("TIOSETPORTSET failed ret %x\n", ret);
						abort();
					}
					//break;
				}
			}

			if(port_args->mode.rx == FILE_MODE)
			{
				fwrite(&pBuf[i], 1, 1, port_args->fp);
				fflush(port_args->fp);
			}
			else if(port_args->mode.rx == LOOPBACK)
			{
				inBuf[curPos] = pBuf[i];
				curPos++;
				if(curPos == packetSize)
				{					
					if(!memcmp(inBuf, pakBuf, port_args->packetSize))
						goodPacket++;
					else
					{
						dataMismatch++;
						for(j=0;j<port_args->packetSize; j++)
						{
							printf("%c%c",inBuf[j], pakBuf[j]);
							if(inBuf[j] != pakBuf[j])
							{
								printf("\nmismatch index %d %c%c 0x%x 0x%x\n",j, inBuf[j], pakBuf[j],inBuf[j], pakBuf[j]);
								//break;
								return 0;
							}
						}
					}
					
					curPos = 0;
				}
			}
			else if(port_args->mode.rx == RAW_MODE)
			{
				printf("%c",pBuf[i]);
				fflush(NULL);
			}
		}

		if(!pakRecv)
		{
			continue;
		}
		else
		{
			pakRecv = FALSE;
			curNumbytes = 0;
		}

		if(port_args->delayRx != -1)
		{
			usleep(port_args->delayRx);
		}
	}

  return NULL;
}

// transmit thread for each port
void *thread_write(void *arg) {
	// data buffer
	char buf[MAX_BUF_SIZE];
	// old running mode
	int oldMode;
	 struct _port_args *port_args = arg;
	// total number of bytes transmitted
	unsigned long bytes;
	// retutrn value of an api
	int ret;
	// vars to use with select api
	fd_set tmpfds;
	fd_set writefds;
	int fdwidth;

	FD_ZERO(&writefds);
	FD_SET(port_args->fd, &writefds);
	fdwidth=1;
	fdwidth+=port_args->fd;

	Begin:
	bytes = 0;
	while(port_args->mode.tx < 0)
	{
		usleep(1000);
		if (port_args->exit)
			return;
	}

	oldMode = port_args->mode.tx;


	while(1) {
		if(oldMode != port_args->mode.tx){
			goto Begin;
		}
		if(port_args->mode.tx == FILE_MODE)
		{
			while(!feof(port_args->fp))
			{
				if(port_args->use_select > 0)
				{
					tmpfds = writefds;
					ret = select(fdwidth, NULL, &tmpfds, NULL, NULL);
					if(ret <= 0)
						break;
				}
				ret = fread(buf, 1, port_args->packetSize, port_args->fp);
				ret = write(port_args->fd, buf, ret);
				bytes += ret;
				if(port_args->delayTx != -1)
					usleep(port_args->delayTx);
			}
			fseek(port_args->fp, SEEK_SET, SEEK_SET);
			if(port_args->sendOnce == 'y')
			{
				port_args->mode.tx = -1;
				fclose(port_args->fp);
				goto Begin;
			}
			bytes = 0;
		}
		else
		{			
			if(port_args->mode.rx == LOOPBACK)
				if(goodPacket != txPackets)
				{
					//printf("%d %d\n",goodPacket, txPackets);
					continue;
				}
			txPackets++;
			ret = write(port_args->fd, pakBuf, port_args->packetSize);
			if(ret != port_args->packetSize)
				printf("write ret %d\n",ret);
			bytes += ret;
			send_byte[port_args->port]=bytes;
			if(port_args->mode.tx != LOOPBACK)
				printf("\rsent %ld bytes", bytes);
			if(port_args->delayTx != -1)
					usleep(port_args->delayTx);
						
		}
	}

  return NULL;
}

// initializes a port
int start_port( struct _port_args *port_args, BHN_PortSettings *pset, pthread_t *h_read, pthread_t *h_write, int port)
{

	char port_str[20];
	int ret;
	int mode = O_RDWR;


	if(port_args->non_block_io)
		mode |= O_NONBLOCK;

	sprintf(port_str,"/dev/csync%d",pset->port+1);
	port_args->fd = open(port_str, mode);
	if( port_args->fd == -1) {
		printf(" %s open error...\n",port_str);
		abort();
	}

	ret = ioctl(port_args->fd, TIOSETPORTSET, pset);
	if (ret != 0) {
		printf("TIOSETPORTSET failed ret %x\n", ret);
		abort();
	}


	if ( pthread_create( h_read, NULL, thread_read, port_args) ) {
	    printf("error creating thread.");
	    abort();
	}

	if ( pthread_create( h_write, NULL, thread_write, port_args) ) {
	    printf("error creating thread.");
	    abort();
	}

	return 1;
}

int main() {

	int  i;
	// port operation struct vars
	struct _port_args port_args1={0}, port_args2={0};
	
	// pthread handles
	pthread_t h_read1;
	pthread_t h_write1;
	pthread_t h_read2;
	pthread_t h_write2;

	// command index
	int cmdID;
	// command string
	char cmdStr[100];
	// retutrn value of an api
	int ret;
	// set/reset value for commands
	int set_reset;
	// character input
	char c;
	struct set_reg_struct set_reg;
	unsigned int reg;

	printf("%s",version);

	while(1)
	{
		printf("Non-block io?y/n ");
		scanf("%c",&c);
		if(c == 'y')
		{
			port_args1.non_block_io = 1;
			break;
		}
		else if (c == 'n')
			break;
	}
	while(1)
	{
		printf("Print events?y/n ");
		scanf("%c",&c);
		if(c == 'y')
		{
			port_args1.show_events = 1;
			break;
		}
		else if (c == 'n')
			break;
	}
	while(1)
	{
		printf("Use select API?y/n ");
		scanf("%c",&c);
		if(c == 'y')
		{
			port_args1.use_select = 1;
			break;
		}
		else if (c == 'n')
			break;
	}

	memcpy(&port_args2, &port_args1, sizeof(port_args1));
	port_args1.mode.rx = -1;
	port_args1.mode.tx = -1;
	port_args2.mode.rx = -1;
	port_args2.mode.tx = -1;


	if((ret = ReadCfgFile("comsync.cfg",&pset1)) != SYNC_PARAMS)
	{
		printf("Failed to parse cfg file; parameter found %d, required %d\n", ret, SYNC_PARAMS);
		abort();
	}
	memcpy(&pset2, &pset1, sizeof(pset1));
	start_port(&port_args1, &pset1, &h_read1, &h_write1, 0);
	pset2.port = pset1.port + 1;

// the following code shows how to use TIOSETREGISTER and TIOGETREGISTER
#if 0
	set_reg.offset = 4;
	set_reg.value = 0x55;
        ret = ioctl(port_args1.fd, TIOSETREGISTER, &set_reg);
        if (ret != 0) {
               printf("TIOSETREGISTER failed ret %x\n", ret);
               return 0;
        }
	reg = 4;
	ret = ioctl(port_args1.fd, TIOGETREGISTER, &reg);
        if (ret != 0) {
               printf("TIOSETREGISTER failed ret %x\n", ret);
               return 0;
        }

	printf("REG4 value %x\n",reg);
#endif
	for (i = 0; i < (int)MAX_BUF_SIZE; i++)
    {
 		pakBuf[i] = (unsigned char) i % 26 + 0x41; 		
    }

	while(TRUE)
	{

		printf("%s",CMD_USE);

		scanf("%s",cmdStr);
		cmdID = atoi(cmdStr);
		if(cmdID == 14)
			goto Exit;

		switch(cmdID)
		{
		case 1:
			set_reset = 1;
			ret = ioctl(port_args1.fd, TIOSETCLRRTS, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRRTS failed ret %x\n", ret);
				return 0;
			}
			break;
		case 2:
			set_reset = 0;
			ret = ioctl(port_args1.fd, TIOSETCLRRTS, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRRTS failed ret %x\n", ret);
				return 0;
			}
			break;
		case 3:
			set_reset = 1;
			ret = ioctl(port_args1.fd, TIOSETCLRDTR, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRDTR failed ret %x\n", ret);
				return 0;
			}
			break;
		case 4:
			set_reset = 0;
			ret = ioctl(port_args1.fd, TIOSETCLRDTR, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRDTR failed ret %x\n", ret);
				return 0;
			}
			break;
		case 5:
			set_reset = 1;
			ret = ioctl(port_args1.fd, TIOSETCLRBRK, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRBRK failed ret %x\n", ret);
				return 0;
			}
			break;
		case 6:
			set_reset = 0;
			ret = ioctl(port_args1.fd, TIOSETCLRBRK, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRBRK failed ret %x\n", ret);
				return 0;
			}
			break;
		case 7:
			if(pset1.port > 2)
			{
				printf("starting port number %d (max is 2) exceeded for purge\n",
					pset1.port);
				continue;
			}
			port_args2.exit = 0;
			start_port(&port_args2, &pset2, &h_read2, &h_write2, 0);
			sleep(1);
			printf("\n\ndoing purge Rx between port %d and port %d \nflushing existing data\n\n",pset1.port+1,pset2.port+1);
			recv_byte[port_args1.port] = 0;
			send_byte[port_args2.port] = 0;
			port_args2.delayRx = 0;
			port_args2.delayTx = 0;
			port_args2.packetSize = 4096;
			port_args2.mode.tx = COMMAND;

			port_args1.delayRx = 0;
			port_args1.delayTx = 0;
			port_args1.packetSize = 4096;
			port_args1.mode.rx = COMMAND;
			sleep(2);
			ret = tcflush(port_args1.fd, TCIFLUSH);
            if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }
			port_args2.mode.tx = -1;
			port_args1.mode.rx = -1;
			sleep(2);
			printf("\nreceived %ld\n\n", recv_byte[port_args1.port]);
			if(recv_byte[port_args1.port] < send_byte[port_args2.port])
				printf("Purge Rx passed\n\n");
			else
				printf("Purge Rx failed\n\n");
			port_args2.exit = 1;
			close(port_args2.fd);
			sleep(1);
			break;

		case 8:
			if(pset1.port > 2)
			{
				printf("starting port number %d (max is 2) exceeded for purge\n",
					pset1.port);
				continue;
			}
			port_args2.exit = 0;
			start_port(&port_args2, &pset2, &h_read2, &h_write2, 0);
			sleep(1);
			printf("\n\ndoing purge Tx between port %d and port %d \nflushing existing data\n\n",pset1.port+1,pset2.port+1);
			recv_byte[port_args2.port] = 0;
			send_byte[port_args1.port] = 0;
			port_args2.delayRx = 0;
			port_args2.delayTx = 0;
			port_args2.packetSize = 4096;
			port_args2.mode.rx = COMMAND;

			port_args1.delayRx = 0;
			port_args1.delayTx = 0;
			port_args1.packetSize = 4096;
			port_args1.mode.tx = COMMAND;
			sleep(2);
			ret = tcflush(port_args1.fd, TCOFLUSH);
			if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }
			port_args1.mode.tx = -1;
			port_args2.mode.rx = -1;
			sleep(2);
			printf("\nreceived %ld\n\n", recv_byte[port_args2.port]);
			if(recv_byte[port_args2.port] < send_byte[port_args1.port])
				printf("Purge Tx passed\n\n");
			else
				printf("Purge Tx failed\n\n");
			port_args2.exit = 1;
			close(port_args2.fd);
			sleep(1);
			break;

		case 9:
			if(pset1.port > 2)
			{
				printf("starting port number %d (max is 2) exceeded for purge\n",
					pset1.port);
				continue;
			}
			port_args2.exit = 0;
			start_port(&port_args2, &pset2, &h_read2, &h_write2, 0);
			sleep(1);
			printf("\n\ndoing purge Tx/Rx between port %d and port %d \nflushing existing data\n\n",pset1.port+1,pset2.port+1);
			recv_byte[port_args1.port] = 0;
			send_byte[port_args1.port] = 0;
			recv_byte[port_args2.port] = 0;
			send_byte[port_args2.port] = 0;
			port_args2.delayRx = 0;
			port_args2.delayTx = 0;
			port_args2.packetSize = 4096;
			port_args1.mode.rx = COMMAND;
			port_args2.mode.tx = COMMAND;

			port_args1.delayRx = 0;
			port_args1.delayTx = 0;
			port_args1.packetSize = 4096;
			port_args1.mode.rx = COMMAND;
			port_args1.mode.tx = COMMAND;
			sleep(2);
			ret = tcflush(port_args1.fd, TCIOFLUSH);
            if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }
			port_args1.mode.tx = -1;
			port_args1.mode.rx = -1;
			port_args2.mode.tx = -1;
			port_args2.mode.rx = -1;
			sleep(2);
			printf("\nreceived %ld\n\n", recv_byte[port_args1.port]);
			if((recv_byte[port_args1.port] < send_byte[port_args2.port]) && (recv_byte[port_args2.port] < send_byte[port_args1.port]))
				printf("Purge Tx/Rx passed\n\n");
			else
				printf("Purge Tx/Rx failed\n\n");
			port_args2.exit = 1;
			close(port_args2.fd);
			sleep(1);
			break;

		case 10:
			ret = ioctl(port_args1.fd, TIOCSERGETMSR, &set_reset);
			if (ret != 0) {
				printf("TIOSETCLRBRK failed ret %x\n", ret);
				return 0;
			}
			printf("MSR 0x%x\n",set_reset);
			break;

		case 11:
			ret = tcflush(port_args1.fd, TCIOFLUSH);
            if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }
			port_args1.mode.rx = -1;
			port_args1.delayRx = -1;
			port_args1.delayTx = 0;
			GetDataOpParams(&port_args1, TX);
			while(1)
			{
				if(port_args1.mode.tx < 0)
					break;
				sleep(1);
			}
			break;

		case 12:
			ret = tcflush(port_args1.fd, TCIOFLUSH);
            if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }
			port_args1.mode.tx = -1;
			port_args1.delayTx = -1;
			port_args1.delayRx = 0;
			GetDataOpParams(&port_args1, RX);

			fflush(NULL);
			while(1)
			{
				if(port_args1.mode.rx < 0)
					break;
				sleep(1);
			}
			break;

		case 13:
			ret = tcflush(port_args1.fd, TCIOFLUSH);
            if (ret == -1) {
            	printf("tcflush failed ret %x\n", ret);
				return 0;
            }


			GetDataOpParams(&port_args1, TX_RX);
			printf("Port  Totalpacket  Goodpacket  Datamismatch  txPackets\n");
			while(1)
			{
				sleep(1);

					printf("\r%4d%13ld%12ld%14ld%14ld", pset1.port+1,
						goodPacket + dataMismatch,
						goodPacket, dataMismatch,txPackets);
					fflush(NULL);
					if(dataMismatch)
						return 0;
			}
			break;

		case ESC:
			break;
		default:
			printf("wrong command\n");
			break;
		}
	}

	close(port_args1.fd);

	Exit:

	return 0;
}
