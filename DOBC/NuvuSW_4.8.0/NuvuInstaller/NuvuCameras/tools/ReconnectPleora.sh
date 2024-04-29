#!/bin/bash
# ReconnectPleora.sh
#-----------------------
# - list available Ethernet interfaces if one has not been specified
# - reconfigure a specified Ethernet adapter to suit Pleora
# - requires root permissions

###################################################################################
# NOTE: To determine the INTERFACE associated with a physical Ethernet jack 
#        you may be able to use the following command (with root permissions) :
# 	 	ethtool -p $INTERFACE 10
# 	 to force the associated 'link speed' light to blink for 10 seconds
# 	 (this works more reliably on Intel NICs).  
###################################################################################


###############
#Configuring network interface :
INTERFACE='ethX' # Modify this to select the network interface used by the camera / controller

JUMBO_MTU=8164 # Jumbo packets are supported by Pleora interfaces; choose a size that suits your network.

INET_MASK_BITS=16  # Pleora IP Engine expects a Class B network (first pair of the quad)
SUBNET='169.254.' # Pleora IP Engine defaults to this subnet 
IPV4_ADDR="${SUBNET}42.42" # The trailing part of the IP is arbitrary (but shouldn't match the IP Engine's!)
INET_ADDR="${IPV4_ADDR}/${INET_MASK_BITS}"


###############
# Helpers
function ethernetInterfaceList 
{
	ip link show | grep -B1 'link/ether' | grep -v 'link' | cut -d':' -f 2
}

function interfaceInetAddr # <INTERFACE>
{
	local interface=$1

	ip -f inet addr show $interface | grep inet | cut -d' ' -f 6
}

###############
# Process
# Verify chosen interface
if 	! ip link show $INTERFACE &> /dev/null
then 
	echo "Interface $INTERFACE is not available"
	echo " to apply IP address ${INET_ADDR}"
	echo "Valid interfaces [present IP address] are as follows :"
	#ip link show | grep -B1 'link/ether' | grep -v 'link' | cut -d':' -f 2
	for eth in $(ethernetInterfaceList)
	do
		echo -e "\t $eth\t [$(interfaceInetAddr $eth)]"
	done
	exit 1;
fi

# Verify root permissions required for manipulation of network interfaces
# NOTE: root permissions necessary for modprobe ...
WHO=$(whoami)
if [ ! "$WHO" = 'root' ]
then
    	echo -e "\n*** Please run this script as root. ***"
	exit 1
fi


#==========================
# CONFIGURATION TASKS
#Increasing the size of the socket buffers
SOCKET_BUF_SCRIPT=set_socket_buffer_size.sh

# Apply relaxed reverse path filtering to the interface:
#  can help in debugging communication with devices that find themselves on some other subnet.
# NOTE: Pleora provide their own script but it is rather heavy-handed as it does not specify a particular interface.
# REV_PATH_SCRIPT=set_rp_filter.sh
function RelaxSubnetFiltering
{
	echo "* Relaxing reverse path filtering for interface $INTERFACE *"
	sysctl -w net.ipv4.conf.${INTERFACE}.rp_filter=2
}


# Use firewalld (if present) to relax the firewall for the given interface:
#  it is expected that the network for the camera will be dedicated to that purpose
#  and therefore that any devices accessed on that interface are completely trusted. 
# Default zone is intended to be the default firewalld 'trusted' zone
#  but another may be specified optionally.
function SetFirewallZone # [FIREWALLD_ZONE]
{
	local required_zone=trusted
	if 	test -n "$1"
	then 
		required_zone=$1
	fi

	local firewall_bin=$(which firewall-cmd)

	# NOTE: test -x on an empty string returns true, so also test path non-empty. 
	if test "${firewall_bin}" -a -x ${firewall_bin}
	then 
		echo "* Enforcing appropriate firewall settings for interface $INTERFACE *"
		if  	! firewall-cmd --zone=${required_zone} --query-interface=$INTERFACE &> /dev/null
		then # Interface not yet associated with the appropriate zone 
			if  	! firewall-cmd --zone=${required_zone} --add-interface=$INTERFACE
			then 
				echo "Firewall settings unchanged for interface $INTERFACE"
			fi
		else
			echo "Firewall settings unchanged for interface $INTERFACE"
		fi

		echo -n -e "\n" 

		local zoned_interfaces="$(firewall-cmd --zone=${required_zone} --list-interfaces)"
		local surplus=false

		echo "The '${required_zone}' firewall zone contains the following interfaces :"
		for netif in $zoned_interfaces
		do
			echo -n -e "\t $netif"
			if 	test "$netif" = $INTERFACE
			then
				echo -e "\t as required"
			else
				echo -e "\t may be unprotected!"
				surplus=true
			fi
		done				

		if $surplus
		then
			echo -e "Surplus interfaces in the '${required_zone}' firewall zone may be removed via :"
			echo -e "\t firewall-cmd --zone=${required_zone} --remove-interface=<INTERFACE>"
		fi

	else
		echo "* Firewall settings unchanged for interface $INTERFACE *"
	fi
}


# Print the maximum of the ring buffer type as defined by 'ethtool -g $INTERFACE'
function PrintRingBufferMaximum # <RING_BUFFER_TYPE>
{
	local ring_type=$1
	local ring_max=$(ethtool -g ${INTERFACE} | grep -A 4 'Pre-set maximums:' | grep -E -m 1 "^${ring_type}:" | cut -d':' -f 2 | sed 's/\s*//')
	echo "$ring_max"
}

# Maximise the ring buffers to allow breathing room for interrupt moderation
function MaximiseRingBuffers 
{
	local rx_descriptors_max=$(PrintRingBufferMaximum "RX")
	local tx_descriptors_max=$(PrintRingBufferMaximum "TX")

	echo -e "\nMaximising ring-buffer sizes for interface $INTERFACE"
	ethtool -G $INTERFACE rx $rx_descriptors_max tx $tx_descriptors_max
}

# Use ethtool to try and put reasonable caps on interrupt rates
function OptimiseInterruptCoalescence
{
	echo -e "\nOptimising interrupt coalescence settings for interface $INTERFACE"
	# Configure interrupt coalescence for lowest latency, unless data already arriving then limit to reasonable ratio for GigE
	ethtool -C $INTERFACE rx-usecs 0 rx-frames 1 rx-usecs-irq 40 rx-frames-irq 2
	# Configure for serial transmission: frames are almost never large, but data rate is slow; commands are at least two characters
	ethtool -C $INTERFACE tx-usecs 0 tx-frames 1 tx-usecs-irq 174 tx-frames-irq 2
}


#Enhance the performance for the Intel driver if available
DRIVER=`ethtool -i $INTERFACE | grep driver`
DRIVER=${DRIVER##'driver: '}

# NOTE: Forcing options on the kernel module is better done at boot 
#        and, anyway, requires intimate knowledge of the device ecosystem to correctly order arguments
MODPROBE_SAFE=false # eBUS_SDK version 4.1 seems unable to recover connectivity to Ethernet interfaces post manipulation of the driver kernel module
# TODO : discover implicated interface on installation; if it uses an appropriate driver, insert a configuration into /etc/modprobe.d/ to set the module options at boot
function SwitchUpKernelMod # [NEW_MOD_SETTINGS]
{
	local mod_settings="$1"

	# NOTE : So as not to completely cripple networking during the update of the driver settings, 
	#         we prefer 'rmmod' over 'modprobe -r' as the latter removes underlying 
	# 	  network interface modules also.
	if 	$MODPROBE_SAFE
	then
		echo "Removing kernel module $DRIVER to adjust settings ..."
		/sbin/rmmod $DRIVER
		echo -e "Restoring kernel module $DRIVER with settings :\n\t$mod_settings"
		/sbin/modprobe $DRIVER $mod_settings

		sleep 1s # Wait for the driver to come up
	else
		echo "Kernel module left in place."
	fi
}



#==========================
# EXECUTE 

echo -e "Modifying interface $INTERFACE ...\n"


# TODO : Choose interface at installation and add a definite alias at that time; hardcode use of the alias here.  
ip link set $INTERFACE down

echo -n "* " 
$(find /opt/pleora/ebus_sdk/ -name ${SOCKET_BUF_SCRIPT})

echo -n -e "\n" 

case $DRIVER in
e1000)
	echo "* Intel PCI driver found ($DRIVER) *" 
	# NOTE: Pleora recommends PCIe over PCI
	echo "*** WARNING: a PCI network interface may not allow sufficient throughput ***" 

	# InterruptThrottleRate mode 3 allows the interrupt rate to vary based upon incoming data rate.  At large rx rate, interrupts are limited to preserve CPU; at small rx rates interrupts may be more frequent to limit latency
	# 	InterruptThrottleRate=3 RxIntDelay=512 RxAbsIntDelay=1024 Speed=1000 RxDescriptors=4096

	SwitchUpKernelMod "InterruptThrottleRate=3 RxIntDelay=512 RxAbsIntDelay=1024 Speed=1000 RxDescriptors=4096"

	MaximiseRingBuffers 

;;
e1000e)
	echo "* Intel PCIe driver found ($DRIVER) *"
	
	# InterruptThrottleRate mode 4 allows the interrupt rate to vary from 2000 to 8000 depending upon in/out balance of traffic
	# 	InterruptThrottleRate=4 RxIntDelay=124 RxAbsIntDelay=1024 TxIntDelay=124 TxAbsIntDelay=1024

	# InterruptThrottleRate mode 3 allows the interrupt rate to vary based upon incoming data rate.  At large rx rate, interrupts are limited to preserve CPU; at small rx rates interrupts may be more frequent to limit latency
	# 	InterruptThrottleRate=3 RxIntDelay=512 RxAbsIntDelay=1024

	SwitchUpKernelMod "InterruptThrottleRate=3 RxIntDelay=512 RxAbsIntDelay=1024"

	MaximiseRingBuffers 

	OptimiseInterruptCoalescence
	

;;
igb)
	echo "* Intel PCIe driver found ($DRIVER) *"

	# NOTE: InterruptThrottleRate for igb is not available in the Linux driver, rather its functionality is exposed via ethtool's interrupt coalescence settings
	SwitchUpKernelMod "RxIntDelay=512 RxAbsIntDelay=1024" 

	OptimiseInterruptCoalescence

	# igb apparently does not offer control over the number of rx descriptors
	# MaximiseRingBuffers 
;;
esac

echo -n -e "\n" 

SetFirewallZone

echo -n -e "\n" 

RelaxSubnetFiltering

echo -e "\n"
echo "* (Re-)activating interface $INTERFACE *"
# Jumbo packets
ip link set $INTERFACE mtu $JUMBO_MTU 
# Use exclusively the chosen address on the appropriate subnet 
ip address flush dev $INTERFACE scope global
ip address add ${INET_ADDR} dev $INTERFACE 
ip link set $INTERFACE up


echo -e "\n ... finished with interface $INTERFACE"

exit 0
