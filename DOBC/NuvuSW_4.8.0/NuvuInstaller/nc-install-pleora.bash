#!/bin/bash
#----------------------
# nc-install-pleora
#----------------------
# Installer providing Nuvu Cameras Pleora plugin 

if [ "$1" = 'force' ] || [ "$1" = 'FORCE' ] \
	|| [ "$1" = '--force' ] || [ "$1" = '-f' ]
then
	FORCE_INSTALL=true
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

# NOTE: Pleora officially supports only Ubuntu and Redhat-based Enterprise Linices.
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

if 	test -z "$PLEORA_PKG"
then # No drivers to install
    	echo -e "\n*** Driver package/installer missing! ***"
	echo -e "\n*** There is a problem with the contents of this installer: please contact Nuvu Cameras. ***"
	exit 1
fi

PLEORA_PATH=${CURR_DIR}/${DRIVER_DIR}/${PLEORA_PKG}
PLEORA_DIR=${PLEORA_PATH%/*}
PLEORA_PKG=${PLEORA_PATH##*/}

PLEORA_TARG=/opt/pleora/ebus_sdk

PLEORA_NETWORK_SCRIPT=/opt/NuvuCameras/tools/ReconnectPleora.sh

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing Pleora GigE Vision driver & libraries"
cd "$PLEORA_DIR"; pwd;

PKG_FMT=${PLEORA_PKG##*.}

DRIVER_INSTALLED=false
if [ "$PKG_FMT" = 'run' ] 
then
	chmod u+x $PLEORA_PKG
	if 	./$PLEORA_PKG \
			|| $FORCE_INSTALL
	then
		DRIVER_INSTALLED=true
	fi
elif [ "$PKG_FMT" = 'rpm' ] 
then
	if 	yum -y install $PLEORA_PKG \
			|| $FORCE_INSTALL
	then
		DRIVER_INSTALLED=true
	fi
elif [ "$PKG_FMT" = 'deb' ] 
then
	if 	dpkg --install $PLEORA_PKG \
			|| $FORCE_INSTALL
	then
		DRIVER_INSTALLED=true
	fi
fi

if  $DRIVER_INSTALLED
then
	PLEORA_VERSION_DEFS=$(find ${PLEORA_TARG}/ -name PvVersion.h)
	PLEORA_VERSION_MAJOR=$(( $(grep MAJOR ${PLEORA_VERSION_DEFS} | cut -d" " -f3-) ))
	PLEORA_VERSION_MINOR=$(( $(grep MINOR ${PLEORA_VERSION_DEFS} | cut -d" " -f3-) ))
	PLEORA_VERSION_SUB=$(( $(grep SUB ${PLEORA_VERSION_DEFS} | cut -d" " -f3-) ))
	PLEORA_VERSION_BUILD=$(( $(grep BUILD ${PLEORA_VERSION_DEFS} | cut -d" " -f3-) ))

	# NOTE: eBUS v.5 arbitrarily migrated set_puregev_env to a wrapper for executables
	#        such that its effect now depends upon the directory in which it is sourced. 
	#       eBUS v.6 renamed it to set_puregev_env.sh
	#        and also seems to install set_puregev_env.sh directly to /etc/profile.d/
	PLEORA_ENV_SCRIPT=$(find ${PLEORA_TARG}/ -name set_puregev_env*) 
	pleora_env_cmd="pushd $(dirname ${PLEORA_ENV_SCRIPT}) > /dev/null && source ${PLEORA_ENV_SCRIPT}; popd > /dev/null"
	#pushd $(dirname ${PLEORA_ENV_SCRIPT}) && source set_puregev_env* && popd # Sets PUREGEV_ROOT and GENICAM_ROOT
	#source $(find ${PLEORA_TARG}/ -name set_puregev_env*)

	echo -e "\n========================="
	echo "Adding environmental variables"
	echo "$pleora_env_cmd" > /etc/profile.d/nc_set_puregev_env.sh
	chmod a=r,u+w /etc/profile.d/nc_set_puregev_env.sh
	source /etc/profile.d/nc_set_puregev_env.sh

	echo -e "\n========================="
	echo "Adding path to shared libraries"
	# TODO : Consider using Pleora's own install_libraries.sh
	echo "${PUREGEV_ROOT}/lib/" > /etc/ld.so.conf.d/nc_pleora.conf
	echo "${GENICAM_ROOT}/bin/Linux64_x64/" >> /etc/ld.so.conf.d/nc_pleora.conf
	chmod 0644 /etc/ld.so.conf.d/nc_pleora.conf

	echo -e "\n\n\n"

	echo -e "\n========================="
	echo -e "Setting-up network connection for compatability with Pleora\n"


	# TODO: systemd service to load eBUS Universal Pro driver at boot

	# TODO: Introduce upstart service (e.g. nc_network_pleora.conf) to enforce network settings
	#        & set NM_CONTROLLED=NO for appropriate eth port.
	#       Would be systemd for most distros now ...
	#       Config script for user to supply custom network interface name?
	cp ${CURR_DIR}/NuvuCameras/tools/ReconnectPleora.sh $PLEORA_NETWORK_SCRIPT
	chmod 0775 $PLEORA_NETWORK_SCRIPT
	SED_SUBSTITUTION="s/ncReplace/${PLEORA_NETWORK_SCRIPT////\\/}\nexit 0/g"

	# The network stack may have been reset during installation of Pleora so we wait for the connection to the repositories to come back up ...
	echo -e "Checking for the network connection\n(this may take a minute) ... "
	network_access=false
	network_access_trial=0
	until [ $network_access_trial -ge 12 ] 
	do
		(( ++network_access_trial ))
		# TODO: Apparently 8.8.8.8 and 8.8.4.4 are Google's primary and back-up DNS servers
		#        and would also make excellent connectivity tests ...
		if 	! ( ping -q -w1 -c1 google.com || ping -q -w1 -c1 cern.ch ) &>  /dev/null
		then 
			sleep 5;
		else
			network_access=true
			break;
		fi
	done

	if $network_access
	then 
		echo "... established."
	else
		echo "... failed."
	fi

	# Manage network settings and support for the PLEORA_NETWORK_SCRIPT  
	if [ "$DISTRO" = 'ubuntu' ]
	then
		# PLEORA_NETWORK_SCRIPT uses ethtool: install it
		# NOTE: firewalld (also used optionally by PLEORA_NETWORK_SCRIPT) 
		#        is not present under Ubuntu 18 but prseumably that means no firewall to configure ... ?
		if ! installed "ethtool"
		then
			if $network_access
			then 
				echo "Installing ethtool"
				apt-get install -y ethtool
			else
				echo "NOTICE! You will need to install ethtool when network access is restored." 
				echo "Hit Enter to continue"
				read dummy
			fi
		fi

		#Boot-time configuration of network adapter to connect to Pleora
		# NOTE: For the user to decide; user's responsibility to set-up PLEORA_NETWORK_SCRIPT correctly first.
		# NOTE: Also systemd based init precludes rc.local unless re-activated by user
		#sed -i -e 's/"exit 0"/ncReplace/g' /etc/rc.local
		#sed -i -e "$SED_SUBSTITUTION" /etc/rc.local 
		#sed -i -e 's/ncReplace/"exit 0"/g' /etc/rc.local

	elif [ "$DISTRO" = 'RHB' ]
	then
		if ! sestatus | grep status | grep -q disabled
		then 
			#Suppressing SELinux 
			# TODO: Verify that this is actually necessary
			cp /etc/selinux/config /etc/selinux/config.bak
			cp ${PLEORA_DIR}/selinux.config /etc/selinux/config 
			echo "SELinux has been set to 'permissive'."
			echo "The original /etc/selinux/config file has been saved as /etc/selinux/config.bak"
		
			# NOTE: firewall moved to systemd under EL7 (so chkconfig iptables is nonsense there)
			#       & there is no indication that Pleora ever recommended disabling the firewall under Linux
			#Permanently disabling the firewall. To re-enable it run: 'chkconfig iptables on'
			#chkconfig iptables off
			#echo "The firewall has been disabled."
		fi

		#Boot-time configuration of network adapter to connect to Pleora
		# NOTE: For the user to decide; user's responsibility to set-up PLEORA_NETWORK_SCRIPT correctly first.
		# NOTE: Also systemd based init precludes rc.local unless re-activated by user
		#sed -i -e 's/"exit 0"/ncReplace/g' /etc/rc.local
		#grep ncReplace /etc/rc.local || echo ncReplace >> /etc/rc.local
		#sed -i -e "$SED_SUBSTITUTION" /etc/rc.local 
	fi

	echo -e "\n*** Edit $PLEORA_NETWORK_SCRIPT to adjust the interface that you will use to connect to the camera."
	# TODO: Notify the user (either here or in the PLEORA_NETWORK_SCRIPT) 
	#        of the possibility to fix the IP address of the Pleora framegrabber via eBUSPlayer;
	# 	 also point out the opportunity to optimise Intel NIC drivers 
	#        (requires more intimate knowledge of the host's network environment than can be reasonably assumed).

	#Disable automatic connection to the internet
	echo -e "\n*** It is important to disable the option Connect Automatically for the network interface used by the camera. This can be done from the System > Preferences > Network Connections application."

	# TODO : chgrp nuvu-cameras $PLEORA_TARG # and (some) underlying directories
	#        &&
	#        chmod g+w $PLEORA_TARG # and (some) underlying directories
	#        at least for building examples ...

	# Return to installer directory
	cd "$CURR_DIR" 

	if [ -z "$DEV_INSTALL" ] || [ "$DEV_INSTALL" = 'no' ]
	then # Not to build but to run ...
		echo -e "\n========================="
		echo "Installing plugin"
		cp -Rf ${PLEORA_DIR}/nc_grab_pleora /opt/NuvuCameras/Plugins/
		chgrp -Rf  nuvu-cameras /opt/NuvuCameras/Plugins/nc_grab_pleora
		chmod -Rf  0775 /opt/NuvuCameras/Plugins/nc_grab_pleora
	fi

	echo -e "\n\n"
	echo "Installation is complete, please reboot the computer for the changes to take effect."

else
	echo -e "\n*** Driver installation failed! ***"
	exit 1
fi
