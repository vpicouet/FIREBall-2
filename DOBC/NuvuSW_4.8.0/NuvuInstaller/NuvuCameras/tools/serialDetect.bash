#!/bin/bash
#
# serialDetect.bash
#
# Uses udevadm to monitor events related to terminal devices 
#  in order to faciliatate the identification of USB-serial adapters
# -------------------------------------------------------------------
# Copyright Nüvü Caméras, Inc. 2018

target_group=nuvu-cameras
max_attempt=10
if [ -n "$1" ]
then 
	if [ $1 -gt 0 ]
	then
		max_attempt=$1
	fi
fi

echo "Usage :"
echo -e "\t $0 [POLLING_PERIOD]\n"

echo "We will monitor activity for addition/removal of terminal devices"
echo " for $max_attempt seconds."
echo -n "- Would you like to continue? [Y/n] "
read ANS
if [ "$ANS" = 'n' -o "$ANS" = 'N' -o "$ANS" = 'no' -o "$ANS" = 'No' -o "$ANS" = 'NO' ]
then
	exit 1
fi

echo -e "\nPlease, plug-in or un-plug your serial device ..."

capture=""
attempt_count=0
while [ -z "$capture" -a $attempt_count -lt $max_attempt ]
do
	monitor_log=udev.log
	udevadm monitor -s tty -p -u > $monitor_log &
	monitor_pid=$!
#echo $monitor_pid #DEBUG
	sleep 1;
	kill $monitor_pid
	attempt_count=$(( $attempt_count + 1 ))
#echo $attempt_count #DEBUG
	capture=$(grep DEVNAME $monitor_log )
#echo $capture #DEBUG
done

#device_path=$(grep DEVNAME $monitor_log | sed s/DEVNAME=//) 
rm -f $monitor_log

if [ -z "$capture" ]
then
	echo " ... no activity detected."
else
	echo " ... thank you."

	echo -e "\nThe following device was detected :"
	device_path=${capture##*=}
	echo -e "\t $device_path"
	permissions_annealing="sudo chgrp $target_group $device_path"

	nuvu_group_exists=$(groups | grep $target_group)
	echo -e "\nTo ensure access to this device"
	if [ -z "$nuvu_group_exists" ]
	then 
		echo " install the Nüvü Cameras SDK and then"
	fi
	echo " execute the following command "
	echo -e " after system start-up \n or plugging in the serial device :"
	echo -e "\n\t $permissions_annealing\n"
fi

