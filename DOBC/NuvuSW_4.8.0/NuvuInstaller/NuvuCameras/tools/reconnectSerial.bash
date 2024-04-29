#!/bin/bash
# reconnectSerial.bash
#-----------------------
# - list available serial devices if a valid one is not speciified
# - change group access to on the specified valid serial device
# - requires root permissions

######################################################################################
# NOTE: To determine the SERIAL_DEVICE associated with a particular physical adapter
#        you may be able to use the script provided at :
#        	/opt/NuvuCameras/tools/serialDetect.bash  
######################################################################################

###############
#Configuring serial device :
SERIAL_DEVICE=/dev/ttySX  # Modify this to select the serial device used to communicate with the camera / controller

TARGET_GROUP=nuvu-cameras

if 	! getent group $TARGET_GROUP > /dev/null
then
	echo "*** Please install the Nüvü SDK. ***"
	exit 1 
fi


SERIAL_PLUGIN_DIR=/opt/NuvuCameras/Plugins/nc_grab_serial
SERIAL_PLUGIN_EXE=updateSerialPortList
SERIAL_PLUGIN_LIST=port_list
if 	[ ! -x $SERIAL_PLUGIN_DIR/$SERIAL_PLUGIN_EXE ]
then
	echo "The plugin for serial-communication only is not installed"
	exit 1
elif 	! $SERIAL_PLUGIN_DIR/$SERIAL_PLUGIN_EXE 
then
	echo "There is a problem with this installation of the Nüvü SDK."
	echo "Please contact Nüvü Caméras for support, "
	echo " including any error reported and the following output :"
	echo "====================================="
	ls -lR $SERIAL_PLUGIN_DIR/
	echo "-------------------------------------"
	ls -lR ${SERIAL_PLUGIN_DIR%/*}/$(readlink $SERIAL_PLUGIN_DIR)/..
	echo "====================================="
	exit 2
fi

# Verify the requested device is available 
SERIAL_DEV_LIST=$(cat $SERIAL_PLUGIN_DIR/$SERIAL_PLUGIN_LIST)
SERIAL_DEV_VALID=false
if [ -c $SERIAL_DEVICE ]
then
	for dev in $SERIAL_DEV_LIST 
	do
		if [ "$SERIAL_DEVICE" == "$dev" ]
		then
			SERIAL_DEV_VALID=true
		fi
	done
fi

# Display if none match those defined by the list
if  [ $SERIAL_DEV_VALID == false ]
then
	echo "Available serial devices [group access] are as follows :"
	for dev in $SERIAL_DEV_LIST 
	do
		# TODO: parse for group owner?
		access_group=$(ls -g $dev | cut -d' ' -f 3)
		echo -e "\t $dev \t[ $access_group ]" 
	done
	echo "The desired device must have group $TARGET_GROUP"
	exit 1;
fi

# Verify root permissions required for manipulation of permissions on serial devices
# NOTE: root permissions necessary for chgrp ...
WHO=$(whoami)
if [ ! "$WHO" = 'root' ]
then
    	echo -e "\n*** Please run this script as root. ***"
	exit 1
fi
 
#==========================
# EXECUTE 
# Enforce access to the port list
chgrp $TARGET_GROUP $SERIAL_PLUGIN_DIR/$SERIAL_PLUGIN_LIST

echo -e "Modifying serial device $SERIAL_DEVICE ...\n"
chgrp $TARGET_GROUP $SERIAL_DEVICE
echo " ... done."

