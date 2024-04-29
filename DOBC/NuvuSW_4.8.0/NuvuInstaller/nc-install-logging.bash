#!/bin/bash
#----------------------
# nc-install-logging.bash
#----------------------
# Installer providing Nuvu Cameras status logging plugin 

if [ ! -f /opt/NuvuCameras/lib/libnuvu.so ]
then
	echo -e "\n*** The Nuvu Cameras SDK does not seem to be installed! ***"
	echo -e "\n*** Please use the  installNuvuCameras.sh script first.***"
	exit 1
fi

# Verify root permissions for installer
WHO=$(whoami)
if [ ! "$WHO" = 'root' ]
then
    	echo -e "\n*** Please run this script as root. ***"
	exit 1
fi

# Identify location
CURR_DIR=$(pwd)

# Verify expected relative path to installer components  
FILE_NAME=${0##*/}
if [ ! -f ./${FILE_NAME} ]
then
    	echo -e "\n*** Please make sure that the location of your current working directory is the same as the location of the installation script and run this script again. ***"
	exit 1
fi

echo -e "\n========================="
echo "Installing camera-status logging plugin"
ln -s ../Extras/StatusLogging/nc_status_log /opt/NuvuCameras/Plugins/
