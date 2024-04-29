#!/bin/bash
#----------------------
# nc-install-edt.bash
#----------------------
# Installer providing Nuvu Cameras EDT plugin 

if [ "$1" = 'force' ] || [ "$1" = 'FORCE' ] \
	|| [ "$1" = '--force' ] || [ "$1" = '-f' ]
then
	FORCE_INSTALL=true
else
	FORCE_INSTALL=false
fi

if [ -z "$DEV_INSTALL" ] || [ "$DEV_INSTALL" = 'no' ]
then
	if [ ! -f /opt/NuvuCameras/lib/libnuvu.so ]
	then
		echo -e "\n*** The Nuvu Cameras SDK does not seem to be installed! ***"
		echo -e "\n*** Please use the  installNuvuCameras.sh script first.***"
		exit 1
	fi
fi

# NOTE: EDT installer is more flexible than this; this check is uneccessarily restrictive ...
# TODO: Use osVersion.bash
#Ubuntu
if cat /etc/*-release | grep DISTRIB_ID | grep -i ubuntu  1>  /dev/null
then
	DISTRO=ubuntu
#Scientific Linux
elif cat /etc/*release | grep -i "scientific linux"  1>  /dev/null
then
	DISTRO=RHB
elif cat /etc/*release | grep -i "centos"  1>  /dev/null
then
	DISTRO=RHB
else 
	echo -e "\n*** This installation script runs only under Scientific Linux, CentOS or Ubuntu. ***"
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

source .dependenciesList.bash # Defines *_PKG variables

if 	test -z "$EDT_PKG"
then # No drivers to install
    	echo -e "\n*** Driver package/installer missing! ***"
	echo -e "\n*** There is a problem with the contents of this installer: please contact Nuvu Cameras. ***"
	exit 1
fi

EDT_PATH=${CURR_DIR}/${DRIVER_DIR}/${EDT_PKG}
EDT_DIR=${EDT_PATH%/*}
EDT_PKG=${EDT_PATH##*/}

EDT_TARG=/opt/EDTpdv

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing EDT CameraLink driver & libraries"
cd "$EDT_DIR"; pwd;

PKG_FMT=${EDT_PKG##*.}

DRIVER_INSTALLED=false
if [ "$PKG_FMT" = 'run' ] 
then 
	chmod +x $EDT_PKG
	# NOTE: Must specify default install directory to silence prompt 
	if 	./$EDT_PKG $EDT_TARG \
		|| $FORCE_INSTALL
	then
		if [ -d $EDT_TARG ]
		then
			cd $EDT_TARG
			# NOTE: Build of vlviewer would require install of Qt dev tools 
			#        but we don't strictly require it.
			#       Nevertheless, if vlviewer fails to build we'll get a 
			#        false negative from the validate*.sh scripts.
			#       However, we can block that check by faking the presence of vlviewer.
			EDT_GUI=vlviewer
			touch $EDT_GUI
			if ./validate_pdvinst.sh || ./validate_install.sh \
				|| $FORCE_INSTALL
			then 
				DRIVER_INSTALLED=true
			fi
			rm -f $EDT_GUI
		fi
	fi
elif [ "$PKG_FMT" = 'rpm' ] 
then
	# TODO: Use yum with local package
	if 	yum -y install $EDT_PKG \
			|| $FORCE_INSTALL

	then
		DRIVER_INSTALLED=true
	fi
fi

if  $DRIVER_INSTALLED
then
	# Build shared library expected by nc_grab_edt.so
	make -B -C $EDT_TARG CFLAGS=-fPIC libpdv.so

	# TODO : chgrp nuvu-cameras $EDT_TARG # and (some) underlying directories 
	#        &&
	#        chmod g+w $EDT_TARG # and (some) underlying directories
	#        at least for building examples ...

	# Return to installer directory
	cd "$CURR_DIR" 

	if [ -z "$DEV_INSTALL" ] || [ "$DEV_INSTALL" = 'no' ]
	then # Not to build but to run ...
		echo -e "\n========================="
		echo "Installing plugin"
		cp -Rf $EDT_DIR/nc_grab_edt /opt/NuvuCameras/Plugins/
		chgrp -Rf nuvu-cameras /opt/NuvuCameras/Plugins/nc_grab_edt
		chmod -Rf 0775 /opt/NuvuCameras/Plugins/nc_grab_edt
	fi

	echo -e "\n\n"
	echo "Installation is complete, please reboot the computer for the changes to take effect."

else
	echo -e "\n*** Driver installation failed! ***"
	exit 1
fi
