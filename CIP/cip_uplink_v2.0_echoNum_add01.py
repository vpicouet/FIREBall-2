# MUX-CIP GND TC PROC 
# VERSION 1.1
# 
# PURPOSE: Take data from Mux Tx, send two bytes at a time wrapped in CIP syntax for uplink:		
#
# CIP SYNTAX: 
#		
#   Syyyy(SP)xxqSyyyy(SP)xxqSyyyy(SP)xxq(CR)(LF)
#   e.g. S0013 01WS0013 01WS0013 01W\r\n
#   S     = ASCII          53h                                        
#   yyyy  = two-byte data word and discrete 
#   (SP)  = space ASCII     20h                                
#   xx    = address in hex 
#   q     = W (ASCII 57h) for data word or K (ASCII 4Bh) for discrete   
#   (CR)  = carriage return ASCII 0Dh 
#   (LF)  = line feed ASCII 0Ah 
#
# MUX BLOCK/PACKET SYNTAX:
# 
#   BYTE 1 - 0X7E ('~') - Header
#   BYTE 2 - 0x00-0x03  - Port Number in Zero-indexed Hex
#   BYTE 3 to N-1: Data bytes - can be anything except 0x7E. 
#   BYTE N - 0x7E ('~') - Footer
#
# NOTES:
# - This version concatenates individual mux packets into 'superpackets' to reduce overhead
# - Integer values of bytes are displayed for ports 0x01 and 0x02 because they send binary data
# - ASCII values of bytes are displayed for ports 0x00 and 0x03 because they are terminal commands
# - Log file is automatically opened and named "uplink_<date>_<time>.log"

import time
import serial
import sys


###############################################################################
## REQUIRED VARIABLES

mux_port = '/dev/ttyUSB0'   #Serial port for mux
cip_port = '/dev/ttyUSB1'   #Serial port for cip

mux_speed = 38400           #Baud rate for mux
cip_speed = 1200            #Baud rate for cip

c = ""                      #Global variable to store most recent byte
R = ""                      #Response string for CIP responses
p = []                      #Individual Mux packet
sp = []                     #Concatenation of individual Mux packets

echo = 0                    #Flag for echo response printing purposes
sigerr = 0                  #Flag for packet structure errors

echoCounter = 0

tmax = 0.3                  #Maximum time to wait for additional bytes from a por
max_length = 128 + 3        #Max length for proper Mux packets (data + overhead)
max_uplink = 50             #Threshold beyond which code will throw an error rather than uplink

ports = [chr(i) for i in range(4)] #Valid MUX port values

dev_name = {"00":"DET", "01":"GD1", "02":"NPT", "03":"GD2" } #String names for ports

cip_cmds = []
cip_rsps = []
echoes = []
last_echo = 0

###############################################################################
## METHODS

#Return <date>_<time> string for display 
def timestring(): return time.strftime("%y%m%d_%H:%M:%S")

#Output wrapper for local stdout and log file
def output(s):
    global echo
    print s,
    sys.stdout.flush()
    log.write(s)

#Get representation of byte appropriate for port
def get_byte(char):
    global p,sp
    return str(ord(char))

#Display current byte
def display_byte():
    global c,p,sp
    byte = get_byte(c)
    if len(p)==3 and len(sp)==0: output("\n%s %s %s" % (timestring(),dev_name[p[1].encode('hex')],byte))
    elif c!=0x7e and len(p)>2: output(byte)
    else: pass 
        
#Add current packet to superpacket
def add():
    global add_time,sigerr,p,sp
    sigerr = 0                          # Reset error flag after successful packet assembly 
    if len(sp)==0: sp = p[:-1]          # Start new superpacket if empty
    elif sp[1]==p[1]:                   # If port has not changed
        for x in p[2:-1]: sp.append(x)  # Append data bytes if same port
    else:                               # If this packet is to a different port
        read()                          # Read out current superpacket and reset to empty
        add()                           # Start new superpacket    
    p = []                              # Reset packet
    add_time = time.time()              # Update most recent packet time

    
#Read out current superpacket to CIP Command 
def read():
    global sp,cip_cmds
    sp.append('~')                          #Add '~' to end of superpacket
    if len(sp)%2==1: sp.append('~')         #Add an extra '~' to make length even if needed
    if len(sp)>max_uplink:                  #If superpacket is too long to uplink, output error.
        output("\nWARNING:\n\tMaximum TC length of %i bytes exceeded.\n\t%i bytes sent by port %s.\
        \n\tCowardly refusing to uplink.\n"\
        % (max_uplink,len(sp),sp[1].encode('hex')))
        sp = []
        return
    else:
        output("\n%s CIP Commands:" % timestring())                 #Output header for commands
        for i in range(len(sp)/2):                                  #Run through full superpacket in pairs of bytes.
            i_end = i
            byte1,byte2 = sp[2*i],sp[2*i+1]                         #Get two current bytes          
            dw = "%s%s" % (byte1,byte2)                             #Prepare dataword       
            dws = "".join("{:02x}".format(ord(b)) for b in dw)      #Convert dataword to string
            cip_cmd = "S{0} 01WS{0} 01WS{0} 01W\r\n".format( dws )  #Create CIP command                     
            cip_gnd.write(cip_cmd)                                  #Write command to CIP
            output("\n\t\t%25s %5s %5s" % (cip_cmd[:-2], get_byte(sp[2*i]), get_byte(sp[2*i+1]) )) #Output to log  
            time.sleep(0.2)         
        output("\n")
        global echoCounter
        output(str(echoCounter+i_end*2+1))
        output("\n")
        sp = [] #Restart superpacket

#Error handling for bad bytes         
def signal_error():
    global sigerr,p
    sigerr = 1  
    output("Err\n")
    p = []

###############################################################################
## MAIN CODE 

#Open port for taking data from MUX
mux_tx = serial.Serial(mux_port,
                     baudrate=mux_speed,
                     bytesize=8,
                     parity='N',
                     stopbits=1)

#Open port for sending data to CIP
cip_gnd = serial.Serial(cip_port,
                     baudrate=cip_speed,
                     bytesize=8,
                     parity='N',
                     stopbits=1)

#Open necessary logs
log = open("logs/uplink_%s.log" % timestring(),'w')
cip_log = open("logs/cip_%s.log" % timestring(),'w')

#Output startup info
output("CIP Uplink Version 2.0\n\n")
output("Serial in: MUX Tx on %s at %ibps\n" % (mux_port,mux_speed))
output("Serial out: CIP Command on %s at %ibps\n" % (cip_port,cip_speed))

#Start keeping track of time of last edit to superpacket
add_time = time.time() 

#Main loop
while 1 :

    if mux_tx.inWaiting()>0:                                # If there are any bytes waiting to be read
        
        c = mux_tx.read(size=1)                             # Read in one byte at a time
        
        if c=='~' and len(p)==0:  p = [c]                   # This is header byte. Start packet.
        elif c in ports and len(p)==1: p.append(c)          # This is an address byte. Add to packet.
        elif c!='~' and 1<len(p)<max_length: p.append(c)    # This is a data byte. Add to packet.
        elif c=='~' and len(p)>=3 : p.append(c); add()      # This is a footer byte. Add to packet, then add packet to superpacket.
        else: signal_error()                                # If none of the above - this is an error. 
           
        display_byte()                                      # Display byte properly

    if cip_gnd.inWaiting()>0:                               # If bytes waiting to be read from CIP
        
        r = cip_gnd.read(size=cip_gnd.inWaiting())          # Read in all available bytes
        for c in r:                                         # Print to screen         
            R+=c                                            # Add char to response string
            cip_log.write(c)      
            if c=='\n':   
                if "ECHO" in R: 
                    output("\n%s %s %s" % (echoCounter,R,timestring()))
                    echoCounter+=1 #If a response to uplink
                    if echoCounter > 999: echoCounter = 0
                elif "01/" in R: output("%s %s" % (timestring(),R))
                else: 
                    output(R)
                R = "" #Reset response string for nextline   # If complete response received                
               # if "ECHO" in R: echoes.append(R.split('/')[2][2:])  # Format flight CIP ECHOs
               # elif "06/" in R: cip_rsps.append(R.split('/')[1])   # Format GSE responses
               # else: output(R)
               # R = ""                                              # Reset response string for nextline
  
    if len(sp)>0 and time.time()-add_time>tmax: read()              #Read out superpacket if it has been waiting too long to be read              
