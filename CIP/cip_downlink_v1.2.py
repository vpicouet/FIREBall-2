# MUX-CIP GND TC PROC 
# VERSION 1.1
# 
# PURPOSE: Take data from CIP HH Downlink, pass directly to Mux Rx and display with readable format.	
#
# MUX BLOCK/PACKET SYNTAX:
# 
#   BYTE 1 - 0X7E ('~') - Header
#   BYTE 2 - 0x00-0x03  - Port Number in Zero-indexed Hex
#   BYTE 3 to N-1: Data bytes - can be anything except 0x7E. 
#   BYTE N - 0x7E ('~') - Footer
#
# NOTES:
# - This version does not concatenate mux packets, for transparency of mux behaviour
# - Only integer values of bytes are displayed, to make analysis easier later on
# - Log file is automatically opened and named "downlink_<date>_<time>.log"

import time
import serial
import sys

###############################################################################
##  REQUIRED VARIABLES

cip_port = '/dev/ttyUSB2' #CIP HH Port
mux_port = '/dev/ttyUSB3' #GND MUX Rx Port

cip_speed = 38400 #CIP HH baud rate
mux_speed = 38400 #GND MUX Rx baud rate

p = []     #Store current MUX packet until display
read = 1   #Set to 0 for raw printout, 1 for neat printout

dev_name = {"00":"DET", "01":"GD1", "02":"NPT", "03":"GD2" } #String names for ports

ports = [chr(i) for i in range(4)] #Valid MUX port numbers

###############################################################################
##  METHODS

def timestring(): return time.strftime("%y%m%d_%H:%M:%S")
                   
def output(s):
    print s,
    log.write(s)

def display_packet():
    global p,read,sigerr
    sigerr = 0
    for i,pi in enumerate(p): p[i] = pi.encode('hex')
    if read==1: #Format packet list for readable display          
        p[0] = dev_name[p[1]]                             
        p = p[:-1] #Crop footer 0x7e                                                                                
        s =  str(p)[1:-1].replace(',','').replace('\'','') #Remove [ , and '
    else: s = str(p) #If not in readable mode, just use full packet   
    output("\n%s %s" % (timestring(),s))
    sys.stdout.flush()
    p = []
    
#Error handling for bad bytes goes here. Currently not very sophisticated...           
def signal_error():
    global sigerr,p,c
    sigerr = 1  
    if sigerr==0: output("Err - %i " % ord(c)),
    else: output("%i " % ord(c) )
    p = []
    sys.stdout.flush()


###############################################################################
##  MAIN CODE

#Open log file
log = open("logs/downlink_%s.log" % timestring(),'w')

#Port for sending data to CIP
cip_hh = serial.Serial(cip_port,
                     baudrate=cip_speed,
                     bytesize=8,
                     parity='N',
                     stopbits=1)
                     
#Port for taking data from MUX
mux_rx = serial.Serial(mux_port,
                     baudrate=mux_speed,
                     bytesize=8,
                     parity='N',
                     stopbits=1)

#Output starting message
output("CIP Downlink Monitor Version 1.2\n%s\n" % time.asctime())
output("---------------------------------\n")
output("Serial in: CIP HH on %s at %ibps\n" % (cip_port,cip_speed))
output("Serial out: MUX Rx on %s at %ibps\n\n" % (mux_port,mux_speed))

while 1 :

    if cip_hh.inWaiting()>0:#If any bytes waiting to be read
        
        c = cip_hh.read(size=1) #Read byte from downlink
                
        mux_rx.write(c) #Pass byte straight to Mux, no editing

        if c=='~' and len(p)<=1: p = [c] #This is header byte

        elif c in ports and len(p)==1: p.append( c ) #This is an address byte

        elif c!='~' and 1<len(p)<128+3: p.append( c ) #This is a data byte

        elif c=='~' and len(p)>=3 : #This is footer byte
          
            p.append( c ) #Add byte to packet     
        
            display_packet() #Display packet

        else: signal_error() #Handle error 

