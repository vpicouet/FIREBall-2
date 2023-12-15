File:	ReadMe.txt
Date:	July 28 2017 ver 1.15

Comsync/104, Comsync/PCI and Comsync-III Driver for Linux

Test and utility files :
	runsync		- utility to install the module
	delsync		- utility to uninstall the module
	CSPCI_pset.h	- header file for port configuration
	ioctls.h		- header file for ioctls.
	test.c			- sample test application

TABLE OF CONTENTS
1) INTRODUCTION
2) INSTALLATION INTRODUCTION
3) DRIVER INSTALLATION
4) PORT CONFIGURATION
5) RECENT RELEASES & ENHANCEMENTS TO THE DRIVER
6) CONTACTING TECHNICAL SUPPORT


1) INTRODUCTION
   The following ReadMe file contains information on how to install
   Connect Tech's Comsync Serial Driver for Linux 


2) INSTALLATION INTRODUCTION:
   This package contains synchronous/asynchronous serial drivers for use on Connect Tech's
   Comsync multiport synchronous/asynchronous serial boards.

   For a more detailed description of our products please visit our WWW site at:
   http://www.connecttech.com


3) DRIVER INSTALLATION:

   The package comes with source code, please build the driver first.
   comsync.ko is an installable module. The default major number is 9 though major number is also
   an argument to the driver startup to suit the major number for a system. 'runsync' utility
   is included to install the module and major number set to 200. The ports created are /dev/csync1 and
   /dev/csync2 etc. Follow the naming scheme as csync1..x since this is important when multiple
   comsync boards installed in the same computer. Base address as 'iobasex' and irq number
   as 'irqx' are accepted as driver start parameter for Comsync/104 board, where 'x' will be 1 to 4 that 
   corresponds board index. Therefore, a maximum of four Comsync/104 boards can be installed.


4) PORT CONFIGURATION

   Use CSPCI_pset.h for port configuration and ioctls.h for various ioctls. 'test.c' shows how to set various
   port settings and port operation. Clock tree of Comsync/104/Comsync-III and Comsync/PCI is ESCC-Clock-Tree.pdf and
   IUSC-Clock-Tree.pdf respectively. Test configuration file *.cfg with '-escc' in the file name are for
   Comsync/104 board otherwise for Comsync/PCI board. There is a diagnostic cfg file 'comsync-escc-hdlc-lloop-diag.cfg'
   for Comsync/104 that runs the port in local loopback for HDLC; rename the file to comsync.cfg and run 'test' application,
   it will show packet status if the board is configured properly for I/O base address register and IRQ.

	The reference clock frequency (ref_freq member of _trclk) for Comsync-III is 11059200, for other board it is 18432000. If 
	you get a board with custom clock then mention the clock frequency accordingly.
	
	Comsync-III is found to miss opening flag when idle condition is Mark in SDLC/HDLC. Therefore,
	occassionaly transmitted packet won't be received. It is suggested not to use flag as idle condition in SDLC/HDLC.
	
	Comsync-III read request in SDLC/HDLC mode is handled in packet; the application should pass sufficient buffer to get the received
	packet. The maximum size of SDLC/HDLC packet is 16384. If the buffer is less than the packet size then 
	the read request will fail. The last two bytes of the packet is CRC (CCITT). The application can enable CRC status
	insertion in the received data packet as the last byte of the packet by setting 'last_char_crc_stat' to 1. The CRC status
	is a copy of Read Register 1 of ESCC, bit values are defined in cspci_pset.h.
	
5) RECENT RELEASES and ENHANCEMENTS TO THE DRIVER:
  
  Revision 1.15
	1. Added support for setting async 1x/isochronous mode for escc

  Revision 1.14
	1. Linux kernel support for up to 4.11.2
	
  Revision 1.13
	1. Kernel panic fix during PCI enumeration in Linux 3.10
	
  Revison 1.12
	1. 64 bit compilation fixed
	
	Revision 1.10 & 1.11 April 02, 2013
	1. Supports Comsync-III board.
	
   Revision 1.09 October 31 2012 
   1.TIOSETREGISTER and TIOSETREGISTER ioctl added to set and get any register value respectively.

   Revision 1.08 August 15 2012 
   1.Added support for 2.6.36+

   Revision 1.07 June 05 2012 
   1.fixed TIOHUNT
   
   Revision 1.06 May 30, 2012
   1.IDLE character setting fixed for IUSC.
 
   Revision 1.05 Feb 01, 2012
   1. A bug in ISR of ESCC fixed for RESIDUE_0

   Revision 1.03 Dec 08, 2010
   The following is a summary of the changes in this release:
   1. Comsync/104 support added.

   Revision 1.00 April 23, 2009

   The following is a summary of the changes in this release:
   1. 1st release.


6) CONTACTING TECHNICAL SUPPORT.
   If you have any problems, questions or suggestions regarding the
   Comsync/PCI, please feel free to contact Connect Tech Inc.
   Connect Tech can be reached in a variety of ways:

   MAIL: Connect Tech Inc.
         42 Arrow Road
         Guelph, Ontario, Canada
         N1K 1S6

   TEL 1 519 836 1291 or 1-800-426-8979 in North America
   FAX 1 519 836 4878

   INTERNET:

   Sales:        sales@connecttech.com
   Tech Support: support@connecttech.com
   WWW:          http://www.connecttech.com


   Be sure to check the support section of our home page for answers to
   technical questions at http://www.connecttech.com.
   Also be sure to browse the knowledge data base. If you don't find what you
   is looking for, please contact the support department and let us know.
   We will be glad to help you.

 --- End of README ---



