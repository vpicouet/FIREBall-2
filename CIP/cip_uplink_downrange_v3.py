# DISCRETE-CIP COMMANDING
# VERSION 1.1
# 
# PURPOSE: Take discrete commands from the user and send it wrapped in CIP syntax for uplink:		
#
# CIP SYNTAX: 
#		
#   Syyyy(SP)xxqSyyyy(SP)xxqSyyyy(SP)xxq(CR)(LF)
#   e.g. S0013 07WS0013 07WS0013 07W\r\n
#   S     = ASCII          53h                                        
#   yyyy  = two-byte data word and discrete 
#   (SP)  = space ASCII     20h                                
#   xx    = address in hex 
#   q     = W (ASCII 57h) for data word or K (ASCII 4Bh) for discrete   
#   (CR)  = carriage return ASCII 0Dh 
#   (LF)  = line feed ASCII 0Ah 

import serial
import sys
import time

###################################################################### 
## REQUIRED METHODS
R = ""                      #Response string for CIP responses

#  Output wrapper for local stdout and log file
def output(s):
    print s,
    sys.stdout.flush()
    cip_log.write(s)
    cip_log.flush()
    
#Return <date>_<time> string for display
def timestring(): return time.strftime("%y%m%d_%H:%M:%S")
    
def sendData(command_hex):
    cip_cmd = "S{0} 01WS{0} 01WS{0} 01W\r\n".format(command_hex) #Create discrete CIP command      
    cip_cmd = cip_cmd.upper()
    #print cip_cmd
    cip_gnd.write(cip_cmd) #Add command to list
    output("\nSent:\t\t%25s" % cip_cmd[:-2])  
    cip_log.write("\n%s %25s" % (timestring(),cip_cmd[:-2] ))

def sendDiscrete(command_hex):
    cip_cmd = "S00{0} 01KS00{0} 01KS00{0} 01K\r\n".format(command_hex) #Create discrete CIP command      
    #print cip_cmd
    cip_gnd.write(cip_cmd) #Add command to list
    output("\nSent:\t\t%25s" % cip_cmd[:-2])  
    cip_log.write("\n%s %25s" % (timestring(),cip_cmd[:-2] ))

################################################################### 
## REQUIRED VARIABLES
 
cip_port = '/dev/ttyUSB1' #Serial port for cip

cip_speed = 1200  #Baud rate for cip

cip_gnd = serial.Serial(cip_port,
                     baudrate=cip_speed,
                     bytesize=8,
                     parity='N',
                     stopbits=1)
                                         
cip_log = open("logs/cip_discrete%s.log" % timestring(),'w')

output("CIP DOWNRANGE UPLINK v1.1\n%s\n" % time.asctime())
output("----------------------\n")
output("Commands are NOT being sent to the CIP!\n")



while 1:
    comdType = raw_input("\nDataword (dw) or discrete command (dc)?: ")
    if comdType == "dw":
        addr = raw_input("\nEnter address (0: DOBC, 1:GDR1, 2:CNES, 3:GDR2) --> ")
        if addr in "0123":
	    command = raw_input("\n\nEnter command (Ex. abcd): ")
	    total_command = command + "~"
	    #print total_command
	    command_hex = "".join("{:02x}".format(ord(b)) for b in total_command) #Convert discrete command to string
	    #print command_hex
	    sendData("{:02x}".format(ord("~"))+"0"+addr)
	    for i in range(len(command_hex)/4):
	        data = "%s" % (command_hex[4*i:4*i+4]) #Prepare dataword    
	    #print command_hex[4*i:4*i+4]
	    #print "\n"
	    #print i
	        sendData(data)

            if len(command_hex)%4 == 2:
                sendData("".join("{:02x}".format(ord(b)) for b in "~~"))   
        else:
            output("Invalid address.")
    elif (comdType == "dc"):
        dcommand = str(raw_input("\n\nEnter command (0-31 dec): "))

	if dcommand in str(range(0,32,1)):    
	    total_dcommand = dcommand
	
	    sendDiscrete(total_dcommand)
	else:
	    output("Invalid")
    else:
    	output("Invalid")
    if cip_gnd.inWaiting()>0:                               # If bytes waiting to be read from CIP
        
        r = cip_gnd.read(size=cip_gnd.inWaiting())          # Read in all available bytes
        for c in r:                                         # Print to screen         
            R+=c                                            # Add char to response string
            cip_log.write(c)      
            if c=='\n':   
                if "ECHO" in R: output("%s %s" % (timestring(),R)) #If a response to uplink
                elif "01/" in R: output("%s %s" % (timestring(),R))
                else: 
                    output(R)

