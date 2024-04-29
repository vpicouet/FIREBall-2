#!/bin/bash
#----------------------
# nc-install-serial.bash
#----------------------
# Installer providing Nuvu Cameras serial-communications-only plugin 

INSTALL_TARGET=/opt
INSTALL_DIRECTORY=NuvuCameras

if [ ! -f $INSTALL_TARGET/$INSTALL_DIRECTORY/lib/libnuvu.so ]
then
	echo -e "\n*** The Nuvu Cameras SDK does not seem to be installed! ***"
	echo -e "\n*** Please use the  installNuvuCameras.sh script first. ***"
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
echo "Installing serial-communications-only plugin"
# Enforce sticky group for port_list file access
chmod g+s $INSTALL_TARGET/$INSTALL_DIRECTORY/Extras/SerialControl/nc_grab_serial
ln -s ../Extras/SerialControl/nc_grab_serial $INSTALL_TARGET/$INSTALL_DIRECTORY/Plugins/

