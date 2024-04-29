#!/bin/bash
#----------
# uninstall.sh
# 	Copyright: Nuvu Cameras, Inc. (2014)
#
# Uninstall script for Nuvu Cameras software 
# - Verify root permissions
# - Determine Liinux distribution
# - Uninstall third-party software and appropriate configuration scripts
# - Remove /opt/NuvuCameras
# - Remove remaining configuration scripts

NUVU_UNINSTALL=yes

INSTALL_DIR=/opt/NuvuCameras
NUVU_GROUP=nuvu-cameras

# Verify root permissions for installer
WHO=$(whoami)
if 	[ ! "$WHO" = 'root' ]
then
    	echo -e "\n*** Please run this script as root. ***"
	exit 1
fi

DISTRO=none
#Ubuntu (12.4)
if 	cat /etc/*-release | grep DISTRIB_ID | grep -i ubuntu  1>  /dev/null
then
	DISTRO=ubuntu
#Scientific Linux
elif 	cat /etc/*release | grep -i "scientific linux"  1>  /dev/null
then
	DISTRO=RHB
elif 	cat /etc/*release | grep -i "centos"  1>  /dev/null
then
	DISTRO=RHB
fi

# Remove third-party acquisition libraries
EDT_UNINSTALL=yes
MATROX_UNINSTALL=yes
PLEORA_UNINSTALL=yes

if 	[ "$EDT_UNINSTALL" = 'yes' ]
then
	EDT_UNINSTALL=no # Pending check that it's actually installed
	if 	[ -x /opt/EDTpdv/uninstall.sh ]
	then 	# Installed ... but how?
		EDT_UNINSTALL=fail
		if 	[ "$DISTRO" = 'ubuntu' ]
		then
			if 	dpkg-query -l edtpdv | grep edtpdv | sed -e 's/\ /\t/' | cut -f 1 | grep -q ii
			then 	# Installed via package manager (probably an alien-converted rpm)
				EDT_UNINSTALL=yes
				if 	! dpkg -P edtpdv
				then
					EDT_UNINSTALL=fail
				fi
			fi
		elif 	[ "$DISTRO" = 'RHB' ]
		then
			if 	rpm -q EDTpdv > /dev/null
			then 	# Installed via package manager
				EDT_UNINSTALL=yes
				if 	! rpm -e EDTpdv
				then
					EDT_UNINSTALL=fail
				fi
			fi
		fi

		if 	[ "$EDT_UNINSTALL" = 'fail' ]
		then 	# The installation is unknown to the package manager
			#  or otherwise could not be handled by it :
		     	#  it must have been installed via a script
		     	#  so we uninstall it likewise.
			# NOTE: uninstall script works on pwd, and leaves some traces behind ...
			EDT_UNINSTALL=yes
			if 	! ( cd /opt/EDTpdv && ./uninstall.sh && rm -Rf * \
					&& cd .. && rmdir EDTpdv
					)
			then
				EDT_UNINSTALL=fail
			fi
		fi
	fi
fi
case $EDT_UNINSTALL in
yes)
	echo "EDT uninstall complete."
;;
fail)
	echo "*** EDT uninstall failed. ***"
;;
esac


if 	[ "$MATROX_UNINSTALL" = 'yes' ]
then
	MATROX_UNINSTALL=no # Pending check that it's actually installed
	if 	[ -x /opt/matrox_imaging/tools/mil-uninstall.sh ]
	then
		MATROX_UNINSTALL=yes
		MATROX_SERIAL_SCRIPT=/opt/NuvuCameras/tools/nc_serial_mil.sh
		if 	[ "$DISTRO" = 'ubuntu' ]
		then
			if ! (  /opt/matrox_imaging/tools/mil-uninstall.sh \
				&& sed -i -e "s/${MATROX_SERIAL_SCRIPT////\\/}//g" /etc/rc.local \
				)
			then
				MATROX_UNINSTALL=fail
			fi
		elif [ "$DISTRO" = 'RHB' ]
		then
			if ! (  /opt/matrox_imaging/tools/mil-uninstall.sh \
				&& rm /etc/sudoers.d/mil_serial \
				&& rm /etc/init/nc_serial_mil.conf \
				&& rm /var/lib/mil \
				&& rm /var/log/mil-installer-64.log \
				)
			then
				MATROX_UNINSTALL=fail
			fi
		fi
	fi
fi
case $MATROX_UNINSTALL in
yes)
	echo "MIL-Lite uninstall complete."
;;
fail)
	echo "*** MIL-Lite uninstall failed. ***"
;;
esac


if 	[ "$PLEORA_UNINSTALL" = 'yes' ]
then
	PLEORA_UNINSTALL=no # Pending check that it's actually installed
	if [ -d /opt/pleora/ebus_sdk ] 
	then 	# Installed ... but how?
		PLEORA_UNINSTALL=fail
		if 	[ "$DISTRO" = 'ubuntu' ]
		then
			if 	dpkg-query -l ebus_sdk_ubuntu-x86_64 | grep ebus_sdk_ubuntu-x86_64 | sed -e 's/\ /\t/' | cut -f 1 | grep -q ii
			then 	# Installed via package manager
				PLEORA_UNINSTALL=yes
				if 	! dpkg -P ebus_sdk_ubuntu-x86_64
				then
					PLEORA_UNINSTALL=fail
				fi
			fi
		elif 	[ "$DISTRO" = 'RHB' ]
		then
			# NOTE: package name specific to distro _version_ because convenience(?)
			PLEORA_PKG_NAME=$(rpm -q -a | grep ebus_sdk)
			if 	rpm -q $PLEORA_PKG_NAME > /dev/null
			then 	# Installed via package manager
				PLEORA_UNINSTALL=yes
				if 	! rpm -e $PLEORA_PKG_NAME
				then
					PLEORA_UNINSTALL=fail
				fi
			fi
		fi

		if 	[ "$PLEORA_UNINSTALL" = 'fail' ]
		then 	# The installation is unknown to the package manager
			#  or otherwise could not be handled by it :
		     	#  it must have been installed via a script
		     	#  so we uninstall it likewise.
			source $(find /opt/pleora/ebus_sdk/ -name set_puregev_env) # Sets PUREGEV_ROOT and GENICAM_ROOT

			if 	[ -n "${PUREGEV_ROOT}" -a -x ${PUREGEV_ROOT}/bin/uninstall.sh ]
			then 
				PLEORA_UNINSTALL=yes
				if 	! ${PUREGEV_ROOT}/bin/uninstall.sh
				then
					PLEORA_UNINSTALL=fail
				fi
				# Pleora's uninstaller is rather overzealous and deletes /opt if it was empty
				if [ ! -d /opt ]
				then
					mkdir /opt
				fi

				if 	[ "$PLEORA_UNINSTALL" = 'yes' ]
				then # So far so good; purge our own configuration tweaks
					PLEORA_NETWORK_SCRIPT=/opt/NuvuCameras/tools/ReconnectPleora.sh # NOTE: Likely to have been modified by user; don't destroy their work
					if 	[ "$DISTRO" = 'ubuntu' ]
					then
						if ! ( rm /etc/ld.so.conf.d/nc_pleora.conf \
							&& rm /etc/profile.d/set_puregev_env.sh
							)
						then
							PLEORA_UNINSTALL=fail
						fi
					elif 	[ "$DISTRO" = 'RHB' ]
					then
						if ! ( rm /etc/ld.so.conf.d/nc_pleora.conf \
							&& rm /etc/profile.d/set_puregev_env.sh \
							&& (if test -f /etc/selinux/config.bak; then mv /etc/selinux/config.bak /etc/selinux/config; fi)
							)
						then
							PLEORA_UNINSTALL=fail
						fi
					fi
				fi
			fi
		fi
	fi
fi
case $PLEORA_UNINSTALL in
yes)
	echo "Pleora eBUS uninstall complete."
;;
fail)
	echo "*** Pleora eBUS uninstall failed. ***"
;;
esac


if 	[ "$NUVU_UNINSTALL" = 'yes' ]
then
	# Remove NuvuCameras directory
	if 	rm -R $INSTALL_DIR
	then
		echo "Successfully removed Nuvu Cameras software and libraries."
	else
		echo "*** Failed to remove $INSTALL_DIR (containing Nuvu Cameras software and libraries). ***"
	fi

	# Undo Nuvu-specific configuration
	if 	rm -f /etc/ld.so.conf.d/nuvu.conf
	then
		echo "Nuvu Cameras library removed from linker path"
	else
		echo "*** Failed to remove Nuvu Cameras library from linker path ***"
	fi

	groupdel $NUVU_GROUP
	GROUP_STATUS=$?
	if 	(($GROUP_STATUS == 8))
	then 
		echo -n "*** Couldn't delete group $NUVU_GROUP because it is the primary group of the following users: "
		for nuvu_user in $(groupmems -l -g $NUVU_GROUP)
		do
			if 	sudo -u $nuvu_user groups | sed -e 's/\ /\t/' | cut -f 1 | grep -q $NUVU_GROUP 
			then
				echo -n "$nuvu_user "
			fi
		done
		echo "***"
	elif 	(($GROUP_STATUS != 0))
	then
		echo "*** Couldn't delete group $NUVU_GROUP. ***"
	else
		echo "Group $NUVU_GROUP deleted."
	fi
fi
	
