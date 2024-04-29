#!/bin/bash
#----------------------
# installNuvuCameras.sh
#----------------------
# Installer providing  Nuvu Cameras SDK 4
# 	for 64-bit Linux:  Scientific Linux & CentOS 6,7; Ubuntu 12.04,16.04,18.04 LTS 
# 	
# Supporting files:
# 	.determineThirdPartyPath.bash
#	.dependenciesList.bash
# 	.choosePluginPrompt.bash
#
# #######################################################################################
#
#		Copyright (c) 2013, Nüvü Camēras Inc., All rights reserved.
#
# #######################################################################################

DEV_INSTALL=no
if [ "$1" = 'DEV' ]
then
	DEV_INSTALL=yes
fi
CAM_EXAMPLE=no
NUPIXEL=no
GRAB_EXAMPLE=no
CCCP_SUITE=no

LOGGING_INSTALL=yes
VIRTUAL_INSTALL=no # In 4.2 causes Ubuntu 16 to hang when plugin is loaded

THIRD_PARTY_INSTALL=yes
THIRD_PATH='/opt' # Default installation directory for commercial third-party software
# TODO: Arguments to pre-select choices (e.g. /opt path, frame grabber)
THIRD_CHOSEN=no

LIB_PATH='NuvuCameras/lib/libnuvu.so'
LIB_PATH_PARSABLE=$( echo ${LIB_PATH} | sed 's/\//\\\//g' )
LIB_VERSION=$( ls ${LIB_PATH}.[1-9].[0-9].[0-9] | sed "s/${LIB_PATH_PARSABLE}.//" )
LIBNUVU_MAJOR=$( echo ${LIB_VERSION} | cut -d . -f 1 )
LIBNUVU_MINOR=$( echo ${LIB_VERSION} | cut -d . -f 2 )
LIBNUVU_PATCH=$( echo ${LIB_VERSION} | cut -d . -f 3 )

QT_MAJOR=4
QT_MINOR=8
QT_PATCH=5
QT_VERSION="${QT_MAJOR}.${QT_MINOR}.${QT_PATCH}"

# Clean terminal.
#printf "\033c"
echo -ne "\ec"

if [ "$DEV_INSTALL" = 'no' ]
then
	echo "======================"
	echo "Nuvu Cameras Installer"
	echo "==== for version ====="
	echo "======= ${LIBNUVU_MAJOR}.${LIBNUVU_MINOR}.${LIBNUVU_PATCH} ========"
	echo "======================"
else
	echo "Configuring build environment ..."
fi


# Verify expected relative path to installer components  
FILE_NAME=${0##*/}
if [ ! -f ./${FILE_NAME} ]
then
    	echo -e "\n*** Please make sure that the location of your current working directory is the same as the location of the installation script and run this script again. ***"
	exit 1
fi

# Identify location
CURR_DIR=$(pwd)

PROC_ARCH=$(uname -p) # Must be x86_64 ...
if [ $PROC_ARCH != 'x86_64' ]
then
	echo -e "\n*** This software requires 64-bit Linux. ***"
	exit 1
fi

DISTRO=none
RHB_VERSION=0
OS_VERSION=''

# TODO: fixed config.bash to allow configurable list of helper script files to source  
source .depList.bash 
source .osVersion.bash
if 	test -z "$OS_VERSION"
then
	echo -e "\n*** Your operating system version does not seem to be supported by this installer. ***"
	echo -n "- Would you like to continue? [y/N] "
	read ANS
	if  ! [ "$ANS" = 'y' -o "$ANS" = 'Y' -o "$ANS" = 'yes' -o "$ANS" = 'Yes' -o "$ANS" = 'YES' ]
	then
		exit 1
	fi
echo -e "\n\n"
fi


# Verify presence of dependency packages
source .depPackages.bash # Defines printPackage functions
source .dependenciesList.bash # Defines paths to dependency packages 

function assertPackage # <DEPENDENCY> <PATH>
{
	local dep=$1
	local path=$2

	local packagePath="$(printPackageDestinationPath $dep)"
	local packageName="$(printPackageName $dep $OS_VERSION)"
	if 	test -n "$packageName"
	then # Expect an actual package
		if 	! ( test -f $path/$packagePath/$packageName )
		then # Package we expect to need is missing
			echo -e "\n*** There is a problem with the contents of this installer (for internal dependency $dep): please contact Nuvu Cameras. ***"
			exit 1
		fi
	fi
}

if [ "$DEV_INSTALL" = 'no' ]
then
	for grabber in $GRABBER_DEPENDENCIES
	do
		assertPackage $grabber "${CURR_DIR}/${DRIVER_DIR}"
	done

	for dependency in $EXTRA_DEPENDENCIES
	do
		assertPackage $dependency "${CURR_DIR}/${DEP_DIR}"
	done
fi


# Verify root permissions for installer
WHO=$(whoami)
if [ ! "$WHO" = 'root' ]
then
    	echo -e "\n*** Please run this script as root. ***"
	exit 1
fi

# Identify user
CURRENT_USER=${SUDO_USER:-${USER:-unknown}}
HOME_DIR=/home/$CURRENT_USER


# Prepare to manage packages
# TODO: Integrate into osVersion.bash?
if [ "$DISTRO" = 'ubuntu' ]
then
	INSTALL_CMD="apt-get install -y"
	function installed { dpkg-query -l $1 | grep "^ii" > /dev/null; }
elif [ "$DISTRO" = 'RHB' ]
then
	INSTALL_CMD="yum install -y"
	function installed { rpm -q $1; }
fi


# Verify access to package manager repositories
if [ "$DISTRO" = 'ubuntu' ]
then
	if ! apt-get update  2>  /dev/null # DEBUG: Doesn't fail on missing network access; on success the term "^Fetched .*$" precedes "Reading package lists... Done$" until what version ... ?
	then 
		echo -e "\n*** The package manager cannot contact its repositories: please make sure that you have an internet connection and run this script again. ***"
		exit 1
	fi
elif [ "$DISTRO" = 'RHB' ]
then
	echo "Proofing package manager :"
	if ! yum makecache  2>  /dev/null
	then 
		echo -e "\n*** The package manager cannot contact its repositories: please make sure that you have an internet connection and run this script again. ***"
		echo -e "\n*** It may also be necessary to verify the contents of your /etc/yum.conf file and /etc/yum.repos.d/ directory. ***"
		exit 1
	fi
	echo -ne "\n"
fi

# Establish source access for (L)GPL licensed libraries
QT_SOURCE_COMMAND=""
if [ "$DISTRO" = 'ubuntu' ]
then
	if [ "$OS_VERSION" = "$UBT_16" ]
	then
		QT_SOURCE_COMMAND="$INSTALL_CMD libqt${QT_MAJOR}-dev" # NOTE: Should provide those below
					#$INSTALL_CMD qt${QT_MAJOR}-qmake
	elif [ "$OS_VERSION" = "$UBT_18" ]
	then
		QT_SOURCE_COMMAND="$INSTALL_CMD libqt${QT_MAJOR}-dev" # NOTE: Should provide those below
					#$INSTALL_CMD qt${QT_MAJOR}-qmake
					#$INSTALL_CMD libqt${QT_MAJOR}-dev-bin
	fi
elif [ "$DISTRO" = 'RHB' ] 
then
	if [ $RHB_VERSION -eq 7 ]
	then
		QT_SOURCE_COMMAND="$INSTALL_CMD qt-devel-${QT_MAJOR}.${QT_MINOR}*"
	fi
fi

# Determine products to install based on installer contents.
product_string="software needed to use the Nuvu SDK" # libnuvu if nothing else

if 	( test -d ${CURR_DIR}/NuvuCameras/CameraExamples)
then
	CAM_EXAMPLE=yes
	product_string="software and examples needed to use a Nuvu camera"
fi


if 	( test -x ${CURR_DIR}/NuvuCameras/bin/NuPixel ) 
then
	NUPIXEL=yes
	product_string="software and application to use a Nuvu camera"
fi


if 	( test -d ${CURR_DIR}/NuvuCameras/GrabExamples)
then
	GRAB_EXAMPLE=yes
	product_string="software and examples to use a Nuvu camera controller"
fi


if 	( test -x ${CURR_DIR}/NuvuCameras/bin/cccpServer \
		&& test -x ${CURR_DIR}/NuvuCameras/bin/cccpComm \
		&& test -x ${CURR_DIR}/NuvuCameras/bin/cccpView \
		) 
then
	CCCP_SUITE=yes
	product_string="software and applications needed to program a Nuvu CCCP"
elif  	( test -x ${CURR_DIR}/NuvuCameras/bin/cccpServer \
		|| test -x ${CURR_DIR}/NuvuCameras/bin/cccpComm \
		|| test -x ${CURR_DIR}/NuvuCameras/bin/cccpView \
		) 
then # A component of the suite is missing ...
    	echo -e "\n*** There is a problem with the contents of this installer: please contact Nuvu Cameras. ***"
	exit 1
fi


if [ "$DEV_INSTALL" = 'no' ]
then
	echo -e "- This process will install the ${product_string}.\n"

	if [ "$NUPIXEL" = 'yes' -o "$CCCP_SUITE" = 'yes' ]
	then
		echo -e "- The GUI of any provided application(s) makes use of Qt libraries\n"
		echo -e "\twhich are made available through your system's package manager\n"
		echo -e "\tand whose use is provided for by the GNU Lesser General Public License v. 3 (LGPL)\n"
		echo -e "\tunder the terms of which you are hereby invited to obtain the libraries' source code\n"
		echo -e "\tvia the following command :"
		echo -e "\t\t$QT_SOURCE_COMMAND"
	fi

	echo -e "- Please update your O.S. and make sure you are currently running the latest kernel before proceeding with the installation.\n"
	echo -e "- It might be possible that you need to reboot the computer during the process.\n"
	echo -e "- The installation process might take several minutes.\n"
	echo -n "- Would you like to continue? [Y/n] "
	read ANS
	if [ "$ANS" = 'n' -o "$ANS" = 'N' -o "$ANS" = 'no' -o "$ANS" = 'No' -o "$ANS" = 'NO' ]
	then
		exit 1
	fi
fi

echo -e "\n\n"


echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
#Installing kernel development packages
KERNEL_VERSION=$(uname -r | sed "s/\.${PROC_ARCH}//")
REBOOT=no
KERNEL_PKG_FAIL=no
KERNEL_EXTRAS_FAIL=no

# Wraps install-check, install attempt and failure handling
#      - Takes installation command, package name and failure warning
#      - Handles REBOOT, KERNEL_EXTRAS_FAIL and KERNEL_PKG_FAIL flags
function installKernelPackageChecked # <INSTALL_CMD> <PKG_NAME> <FAIL_MSG>
{ 
		local install_cmd=$1
		local package=$2
		local fail_msg=$3
		
		if ! installed "$package"
		then 
			REBOOT=yes
			if ! $install_cmd $package
			then
				KERNEL_PKG_FAIL=yes
			fi
		else
			echo 'Nothing to do.'
		fi
						
		if [ "$KERNEL_PKG_FAIL" = "yes" ]
		then
			KERNEL_EXTRAS_FAIL=yes
			echo -e "\n*** $fail_msg ***"
			KERNEL_PKG_FAIL=no # Reset for next test
		fi
	 }

echo "Installing kernel firmware, development and header packages via package manager."

echo "Checking for installed firmware package."
FAIL_MSG="Can't install firmware package for your kernel version"
if [ "$DISTRO" = 'ubuntu' ]
then
	installKernelPackageChecked "$INSTALL_CMD --install-recommends" 'linux-firmware' "$FAIL_MSG"

elif [ "$DISTRO" = 'RHB' ]
then
	if [ $RHB_VERSION -eq 6 ]
	then
		installKernelPackageChecked "$INSTALL_CMD" "kernel-firmware-${KERNEL_VERSION}.noarch" "$FAIL_MSG"
	elif [ $RHB_VERSION -eq 7 ]
	then
		installKernelPackageChecked "$INSTALL_CMD" 'linux-firmware' "$FAIL_MSG"
	fi
fi

echo "Checking for installed development package."
FAIL_MSG="Can't install development package for your kernel version"
if [ "$DISTRO" = 'ubuntu' ]
then
	installKernelPackageChecked "$INSTALL_CMD --install-recommends" 'linux-libc-dev' "$FAIL_MSG"

elif [ "$DISTRO" = 'RHB' ]
then
	installKernelPackageChecked "$INSTALL_CMD" "kernel-devel-${KERNEL_VERSION}.${PROC_ARCH}" "$FAIL_MSG"
fi

echo "Checking for installed headers package."
FAIL_MSG="Can't install headers for your kernel version"
if [ "$DISTRO" = 'ubuntu' ]
then
	installKernelPackageChecked "$INSTALL_CMD --install-recommends" "linux-headers-${KERNEL_VERSION}" "$FAIL_MSG"

elif [ "$DISTRO" = 'RHB' ]
then
	installKernelPackageChecked "$INSTALL_CMD" "kernel-headers-${KERNEL_VERSION}.${PROC_ARCH}" "$FAIL_MSG"
fi

if [ "$KERNEL_EXTRAS_FAIL" = "yes" ]
then
	echo -e "\nSome essential packages are not available for your kernel version ($KERNEL_VERSION)."
	echo "You may need to update your kernel."
	exit 1
fi

echo -e "\n"

#Installing GCC
# TODO: specify version; handle repositories; statically link into lib!
echo "Installing/updating build toolchain via package manager."
if [ "$DISTRO" = 'RHB' ]
then
	if ! installed gcc  >  /dev/null
	then
		REBOOT=yes
	fi
fi
# May still require an update ...
if [ "$DISTRO" = 'ubuntu' ]
then
	$INSTALL_CMD gcc g++
	# TODO: Must we peg the compiler to a specific version with Ubuntu18?

	if [ "$OS_VERSION" = "$UBT_18" ]
	then
		# NOTE: Provides make and is essential (ha-ha) for eBUS_SDK
		$INSTALL_CMD build-essential
	fi

elif [ "$DISTRO" = 'RHB' ]
then
	$INSTALL_CMD gcc gcc-c++
fi

if [ "$REBOOT" = 'yes' ]
then
	clear
	echo "Some kernel modules have been installed."
	echo "You will have to reboot the computer and run the installer again to continue with the installation process."
	exit 1
fi



echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing ncurses (terminal extension) library via package manager."
if [ "$DISTRO" = 'ubuntu' ]
then
	$INSTALL_CMD libncurses5-dev
elif [ "$DISTRO" = 'RHB' ]
then
	$INSTALL_CMD ncurses ncurses-devel
fi



echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Updating Qt runtime environment for GUI."
if [ "$DISTRO" = 'ubuntu' ]
then
	if [ "$OS_VERSION" = "$UBT_12" ]
	then
		echo "Nothing to be done" # TODO: check this 
	elif [ "$OS_VERSION" = "$UBT_16" ]
	then
		if [ "$DEV_INSTALL" = 'yes' ]
		then
			$QT_SOURCE_COMMAND
		else
			echo "Nothing to be done" # TODO: check this 
		fi
	elif [ "$OS_VERSION" = "$UBT_18" ]
	then
		if ! installed "qt${QT_MAJOR}-default"
		then 
			$INSTALL_CMD qt${QT_MAJOR}-default
		fi
		# DEBUG: Really above *and* the following?  Maybe above *or* the following ...
		if [ "$DEV_INSTALL" = 'yes' ]
		then
			$QT_SOURCE_COMMAND
		fi
	fi

elif [ "$DISTRO" = 'RHB' ] 
then

	if [ $RHB_VERSION -eq 6 ]
	then # Need more than base repo for a sufficient Qt version
		if ! installed "qt-$QT_VERSION"
		then # Get qt by hook or crook 
			if [ ! -f /etc/yum.repos.d/kde.repo ]
			then
				#yum-config-manager --add-repo http://kdeforge.unl.edu/apt/kde-redhat/epel/kde.repo
				yum-config-manager --add-repo ${CURR_DIR}/Dependencies/Repositories/kde.repo
			else # Already tried?
				yum-config-manager --enable kde
			fi
			#yum upgrade kdebase # Helps dependencies get over a name-change of this package to kde-baseapps
			#                      ... but likely brings in unnecessary (and potentially conflicting) dependency on samba

			yum update qt # NOTE: Inadequate on EL7; maybe an earlier Qt was present by default on EL6
			yum-config-manager --disable kde
		fi
	elif [ $RHB_VERSION -eq 7 ]
	then
		if ! installed "qt-${QT_MAJOR}.${QT_MINOR}*"
		then 
			$INSTALL_CMD qt-${QT_MAJOR}.${QT_MINOR}* qt-x11-${QT_MAJOR}.${QT_MINOR}* # libQtGui provided by qt-x11
		fi
		# DEBUG: Really above *and* the following?  Maybe above *or* the following ...
		if [ "$DEV_INSTALL" = 'yes' ]
		then
			#$INSTALL_CMD qt-devel-${QT_MAJOR}.${QT_MINOR}*
			$QT_SOURCE_COMMAND
		fi
	fi
fi


echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing GL libraries via package manager."
if [ "$DISTRO" = 'ubuntu' ]
then
	echo "Nothing to be done"
elif [ "$DISTRO" = 'RHB' ]
then
	if [ $RHB_VERSION -eq 7 ]
	then 
		$INSTALL_CMD mesa-libGLU # TODO: check necessity
	fi
fi


echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing CFITSIO (image-file format) library (if necessary)."
CFITSIO_BUILD_LOG=cfitsio_install.log 
CFITSIO_BUILD_DIR=/usr/local/src/cfitsio
CFITSIO_LIB=
CFITSIO_LIB_LINK=shared
CFITSIO_PKG_PATH=${CURR_DIR}/Dependencies/${CFITSIO_PKG} # TODO: Build from an installer configuration variable
if [ "$CFITSIO_LIB_LINK"='shared' ]  
then
	CFITSIO_LIB=/usr/lib/libcfitsio.so
else
	CFITSIO_LIB_LINK=
	CFITSIO_LIB=/usr/lib/libcfitsio.a
fi
	
if [ ! -f $CFITSIO_LIB ]  
then
	#cd /usr/local/src # ${CFITSIO_BUILD_DIR%/*}
	cd ${CFITSIO_BUILD_DIR%/*}
	cp ${CFITSIO_PKG_PATH} . # TODO: Prefer find package file and copy from resulting path?
	echo -n "Extracting "
	tar -x -v -z -f ${CFITSIO_PKG##*/} | while read PROGRESS; do echo -n ' ... '; done; echo 'done.'
	#cd cfitsio # ${CFITSIO_BUILD_DIR##*/}
	cd ${CFITSIO_BUILD_DIR##*/}
	touch $CFITSIO_BUILD_LOG 
	echo -e "\n=========================" | tee -a $CFITSIO_BUILD_LOG 
	echo -n "Configuring " | tee -a $CFITSIO_BUILD_LOG 
	echo ":" >> $CFITSIO_BUILD_LOG 
	#generating makefile with option  --prefix=/usr to control where the library will be installed 
	./configure --prefix=/usr | tee -a $CFITSIO_BUILD_LOG | while read PROGRESS; do echo -n ' ... '; done; echo 'done.'
	echo -e "\n=========================" | tee -a $CFITSIO_BUILD_LOG 
	echo -n "Building " | tee -a $CFITSIO_BUILD_LOG 
	echo ":" >> $CFITSIO_BUILD_LOG 
	#compiling from source
	make $CFITSIO_LIB_LINK | tee -a $CFITSIO_BUILD_LOG | while read PROGRESS; do echo -n ' ... '; done; echo 'done.'
	make install
	make clean >> $CFITSIO_BUILD_LOG

	if [ -f $CFITSIO_LIB ]  
	then
		rm -f $CFITSIO_BUILD_LOG
	else
		echo -e "\n*** Failed to install CFITSIO (please verify previous output and log file ${CFITSIO_BUILD_DIR}/${CFITSIO_BUILD_LOG} ) ***"
	fi

	# Return to installer directory
	cd "$CURR_DIR" 
else
	echo 'Nothing to do.'
fi

echo -e "\n"

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing TIFF (image-file format) library via package manager."
if [ "$DISTRO" = 'ubuntu' ]
then
	if [ "$OS_VERSION" = "$UBT_12" ]
	then
		$INSTALL_CMD libtiff4-dev libtiff4 
		test -e || ln -s /usr/lib/x86_64-linux-gnu/libtiff.so.4 /usr/lib/x86_64-linux-gnu/libtiff.so.3	
	elif [ "$OS_VERSION" = "$UBT_16" ]
	then
		$INSTALL_CMD libtiff5-dev libtiff5 
		#test -e || ln -s /usr/lib/x86_64-linux-gnu/libtiff.so.4 /usr/lib/x86_64-linux-gnu/libtiff.so.3	
	elif [ "$OS_VERSION" = "$UBT_18" ]
	then
		$INSTALL_CMD libtiff5-dev libtiff5 
	fi
elif [ "$DISTRO" = 'RHB' ]
then
	$INSTALL_CMD libtiff libtiff-devel 
fi

echo -e "\n"

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Installing expat (XML parser) library via package manager."
if [ "$DISTRO" = 'ubuntu' ]
then
	$INSTALL_CMD libexpat1
	$INSTALL_CMD libexpat1-dev
	test -e || ln -s /lib/x86_64-linux-gnu/libexpat.so.1 /lib/x86_64-linux-gnu/libexpat.so.0
elif [ "$DISTRO" = 'RHB' ]
then
	$INSTALL_CMD expat-devel
	test -e || ln -s /lib64/libexpat.so.1 /lib64/libexpat.so.0
fi

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Creating nuvu-cameras group; adding current user"
groupadd nuvu-cameras
usermod -a -G nuvu-cameras $CURRENT_USER

if [ "$DEV_INSTALL" = 'no' ]
then
	echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
	echo "Installing Nuvu Cameras libraries and applications"
	echo " "
	rm -f /opt/NuvuCameras -r  
	cp NuvuCameras /opt/ -r

	# cp does not preserve symbolic links
	# TODO: use rsync instead ...
	ln -s /opt/NuvuCameras/lib/libnuvu.so.${LIBNUVU_MAJOR}.${LIBNUVU_MINOR}.${LIBNUVU_PATCH} /opt/NuvuCameras/lib/libnuvu.so.${LIBNUVU_MAJOR}
	ln -s /opt/NuvuCameras/lib/libnuvu.so.${LIBNUVU_MAJOR} /opt/NuvuCameras/lib/libnuvu.so

	chgrp -R nuvu-cameras /opt/NuvuCameras
	# TODO: Use find to set 775 on directories only. 
	chmod -R 0775 /opt/NuvuCameras
	chmod g+s /opt/NuvuCameras/LockFiles

	# TODO: Transfer tools / supporting config files specific to the type of installation and target OS,
	#        generated at installer build à la .dependenciesList.bash

	if [ "$CAM_EXAMPLE" = 'yes' ]
	then
		chgrp -Rf  nuvu-cameras /opt/NuvuCameras/CameraExamples
		chmod -Rf  0775 /opt/NuvuCameras/CameraExamples
	fi

	if [ "$NUPIXEL" = 'yes' ]
	then
		chgrp -f  nuvu-cameras /opt/NuvuCameras/bin/NuPixel
		chmod -f  0775 /opt/NuvuCameras/bin/NuPixel
		ln -s ./bin/NuPixel /opt/NuvuCameras/NuPixel
		chgrp -f  nuvu-cameras /opt/NuvuCameras/NuPixel

		DIR="$HOME_DIR"'/Documents/NuPixel Images'
		mkdir -pv "$DIR"
		chown $CURRENT_USER "$DIR"
		chgrp -Rf  nuvu-cameras "$DIR"
		chmod 0775 "$DIR"

		DIR="$HOME_DIR"'/Documents/NuPixel Camera Settings'
		mkdir -pv "$DIR"
		chown $CURRENT_USER "$DIR"
		chgrp -Rf  nuvu-cameras "$DIR"
		chmod 0775 "$DIR"

		DIR="$HOME_DIR"'/Documents/NuPixel Processing Context'
		mkdir -pv "$DIR"
		chown $CURRENT_USER "$DIR"
		chgrp -Rf  nuvu-cameras "$DIR"
		chmod 0775 "$DIR"
	fi


	if [ "$GRAB_EXAMPLE" = 'yes' ]
	then
		chgrp -Rf  nuvu-cameras /opt/NuvuCameras/GrabExamples
		chmod -Rf  0775 /opt/NuvuCameras/GrabExamples
	fi

	if [ "$CCCP_SUITE" = 'yes' ]
	then
		chgrp -f  nuvu-cameras /opt/NuvuCameras/bin/cccpServer
		chmod -f  0775 /opt/NuvuCameras/bin/cccpServer
		ln -s /opt/NuvuCameras/bin/cccpServer /opt/NuvuCameras/cccpServer
		chgrp -f  nuvu-cameras /opt/NuvuCameras/cccpServer

		chgrp -f  nuvu-cameras /opt/NuvuCameras/bin/cccpComm
		chmod -f  0775 /opt/NuvuCameras/bin/cccpComm
		ln -s /opt/NuvuCameras/bin/cccpComm /opt/NuvuCameras/cccpComm
		chgrp -f  nuvu-cameras /opt/NuvuCameras/cccpComm

		chgrp -f  nuvu-cameras /opt/NuvuCameras/bin/cccpView
		chmod -f  0775 /opt/NuvuCameras/bin/cccpView
		ln -s /opt/NuvuCameras/bin/cccpView /opt/NuvuCameras/cccpView
		chgrp -f  nuvu-cameras /opt/NuvuCameras/cccpView

		if [ "$DISTRO" = 'RHB' ]
		then
			if [ $RHB_VERSION -eq 7 ]
			then 
				:
				# TODO: Notify user of, or otherwise handle, the need to open TCP ports for cccpSuite components to interact.
				#       A cccp-server.xml file in the tools directory defines a firewalld service [needs to be installed UBT18?!?]
				#        that may be added to the 'trusted' zone via the firewall-cmd; 
				# 	 provide a script to run the appropriate commands in tools and document in Installation Guide.
			fi
		fi
	fi
fi

echo -e "\n========================="
echo "Adding path to shared libraries"
echo "/opt/NuvuCameras/lib" >  /etc/ld.so.conf.d/nuvu.conf
chmod 0644 /etc/ld.so.conf.d/nuvu.conf


if [ "$DEV_INSTALL" = 'no' ]
then
	if [ "$LOGGING_INSTALL" = 'yes' ]
	then
		echo -e "\n\n\n"

		source ./nc-install-logging.bash
	fi


	if [ "$VIRTUAL_INSTALL" = 'yes' ]
	then
		echo -e "\n\n\n"
		echo "- You may now install a plugin that simulates access to a camera in software."
		echo -n "- Would you like to continue? [y/N] "
		read ANS
		# NOTE: May simply be an anoyance for the user if they have a physical camera; 
		#        assume 'no' unless explicitly otherwise.
		if [ "$ANS" = 'y' -o "$ANS" = 'Y' -o "$ANS" = 'yes' -o "$ANS" = 'Yes' -o "$ANS" = 'YES' ]
		then
			source ./nc-install-virtual.bash
		fi
	fi
fi

if [ "$THIRD_PARTY_INSTALL" = 'yes' ]
then
	echo -e "\n\n\n"
	echo "- You should now install third-party drivers for at least one of the plugins"
	echo "   that allow access to a camera via the frame-grabber supplied with it."
	echo -n "- Would you like to continue? [Y/n] "
	read ANS
	if [ "$ANS" != 'n' -a "$ANS" != 'N' -a "$ANS" != 'no' -a "$ANS" != 'No' -a "$ANS" != 'NO' ]
	then
		echo -e "\n\n\n"


		if [ "$THIRD_CHOSEN" = 'no' ]
		then
			EDT_INSTALL=no
			MATROX_INSTALL=no
			PLEORA_INSTALL=no

			source .choosePluginPrompt.bash # Sets: EDT_INSTALL, MATROX_INSTALL and PLEORA_INSTALL
		fi


		REQUIRED_SPACE_BLOCKS=2097152 # NOTE: Can be much smaller without MATROX


		echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
		echo "Verifying available space for third-party frame-grabber drivers (which will install to /opt)"

		THIRD_ALT='/home' # Our best bet for an alternative mount point for installation 
		THIRD_LIST="/opt $THIRD_ALT CHOICE" 

		source .determineThirdPartyPath.bash # Uses THIRD_LIST and THIRD_ALT; Sets THIRD_PATH
			

		# Protect existing /opt
		if [ "$THIRD_PATH" != '/opt' ]
		then
			mv /opt /opt_tmp
			NEW_PATH=$(shortestNewPath $THIRD_PATH)
			mkdir -p $THIRD_PATH 
			if [ $NEW_PATH ]
			then
				chgrp -Rf nuvu-cameras $NEW_PATH
				chmod -Rf g+rwx $NEW_PATH
			fi
			ln -s $THIRD_PATH /opt # Redirect installation to /opt to chosen path
		fi

		if [ "$EDT_INSTALL" = 'yes' ]
		then
			cd "$CURR_DIR"
			# NOTE: Post-install checks are guaranteed to FAIL 
			#        because vlviewer build would require Qt dev tools 
			#        which we judge to be too much of an imposition;
			#        use FORCE to ignore the results of those checks 
			#        but at least the output exists for consultation ...
			if 	! DEV_INSTALL=$DEV_INSTALL ./nc-install-edt.bash FORCE
			then
				EDT_INSTALL='fail'
			fi
			echo -e "\n\n\n"
		fi

		MIL_SERIAL_SCRIPT=/opt/NuvuCameras/tools/nc_serial_mil.sh
		if [ "$MATROX_INSTALL" = 'yes' ]
		then
			cd "$CURR_DIR"
			if 	! DEV_INSTALL=$DEV_INSTALL ./nc-install-matrox.bash
			then
				MATROX_INSTALL='fail'
			fi
			echo -e "\n\n\n"
		else
			rm -f $MIL_SERIAL_SCRIPT
		fi

		PLEORA_NETWORK_SCRIPT=/opt/NuvuCameras/tools/ReconnectPleora.sh # TODO: Have this identified in .dependenciesList.bash
		if [ "$PLEORA_INSTALL" = 'yes' ]
		then
			cd "$CURR_DIR"
			if 	! DEV_INSTALL=$DEV_INSTALL ./nc-install-pleora.bash
			then
				PLEORA_INSTALL='fail'
			fi
			echo -e "\n\n\n"
		else
			rm -f $PLEORA_NETWORK_SCRIPT
		fi
	fi

fi

echo -e "\n\n\n"

echo -e "\n>>>>>>>>>>>>>>>>>>>>>>>>>\n>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Finalising / Cleaning up"
# Restore previous /opt and add sym. links as necessary
if [ "$THIRD_PATH" != '/opt' ]
then
	rm /opt # Just a symbolic link
	mv /opt_tmp /opt # Restore original /opt
	for THIRD_PARTY in $(ls -1 $THIRD_PATH)
	do
		ln -s $THIRD_PATH/$THIRD_PARTY /opt # Make installed paths accessible via /opt
	done
fi

# Update default library paths
ldconfig


if [ "$EDT_INSTALL" = 'fail' ]
then
	echo "*** EDT CameraLink plugin installation failed! ***"
fi
if [ "$MATROX_INSTALL" = 'fail' ]
then
	echo "*** Matrox CameraLink plugin installation failed! ***"
fi
if [ "$PLEORA_INSTALL" = 'fail' ]
then
	echo "*** Pleora GigE Vision plugin installation failed! ***"
fi

if [ "$EDT_INSTALL" = 'fail' ] || [ "$MATROX_INSTALL" = 'fail' ] || [ "$PLEORA_INSTALL" = 'fail' ]
then
	echo "*** If no other drivers were installed successfully, you will not be able to use your camera. ***"
fi


echo -e "\n\n\n"
echo "Installation is complete, please reboot the computer for the changes to take effect."

exit 0

