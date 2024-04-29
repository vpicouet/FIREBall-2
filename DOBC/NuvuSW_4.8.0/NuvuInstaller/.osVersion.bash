DISTRO=none
RHB_VERSION=0

if cat /etc/*-release | grep DISTRIB_ID | grep -i ubuntu  1>  /dev/null
then # Ubuntu
	DISTRO=ubuntu

#elif cat /etc/*-release | grep DISTRIB_ID | grep -i linuxmint  1>  /dev/null # TODO: ?
#then # LinuxMint
# 	DISTRO=mint # TODO: Ubuntu-based but different release labelling, so we need to distinguish them below ... and beyond, for Pleora does not support LinuxMint explicitly
elif cat /etc/*-release | grep -i "scientific linux"  1>  /dev/null
then # Enterprise Linux
	DISTRO=RHB
elif cat /etc/*-release | grep -i "centos"  1>  /dev/null
then # also Enterprise Linux
	DISTRO=RHB
else 
	echo -e "\n*** This installation script runs only under Scientific Linux, CentOS or Ubuntu. ***"
	exit 1
fi


if [ "$DISTRO" = 'ubuntu' ]
then
	echo -e "Your system has been identified as Ubuntu.\n"

	if 	cat /etc/*-release | grep DISTRIB_RELEASE | grep -e "12.[[:digit:]]" 1>  /dev/null
	then # release 12.something found
		OS_VERSION=$UBT_12
	elif 	cat /etc/*-release | grep DISTRIB_RELEASE | grep -e "16.[[:digit:]]" 1>  /dev/null
	then # release 16.something found
		OS_VERSION=$UBT_16
	elif 	cat /etc/*-release | grep DISTRIB_RELEASE | grep -e "18.[[:digit:]]" 1>  /dev/null
	then # release 18.something found
		OS_VERSION=$UBT_18
	fi

elif [ "$DISTRO" = 'RHB' ]
then
	echo -e "Your system has been identified as Redhat-based Enterprise Linux (Scientific Linux or CentOS).\n"
	
	if 	cat /etc/*-release | grep -e "release 6.[[:digit:]]" 1>  /dev/null
	then # release 6.something found
		RHB_VERSION=6
		OS_VERSION=$RHB_6
	elif 	cat /etc/*-release | grep -e "release 7.[[:digit:]]" 1>  /dev/null
	then # release 7.something found
		RHB_VERSION=7
		OS_VERSION=$RHB_7
	fi
fi
