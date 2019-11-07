#!/bin/bash

FAMILY=i40e

#service udev start

# Remove old modules (if loaded)
rmmod i40e
rmmod pf_ring

# We assume that you have compiled PF_RING
insmod ../../../../../kernel/pf_ring.ko

# Required by i40e
modprobe ptp
modprobe vxlan
modprobe configfs

# Load the driver
insmod ./i40e.ko

# Enable debugging
#find /sys/kernel/debug/i40e/ -name command -exec sh -c 'echo  "msg_enable 16" > {}' {} ';'

# Disable multiqueue
#find /sys/kernel/debug/i40e/ -name command -exec sh -c 'echo  "set rss_size 1" > {}' {} ';'

sleep 1

killall irqbalance 

INTERFACES=$(cat /proc/net/dev|grep ':'|grep -v 'lo'|grep -v 'sit'|awk -F":" '{print $1}'|tr -d ' ')
for IF in $INTERFACES ; do
	TOCONFIG=$(ethtool -i $IF|grep $FAMILY|wc -l)
        if [ "$TOCONFIG" -eq 1 ]; then
		printf "Configuring %s\n" "$IF"

		# Set number of RSS queues
		ethtool -L $IF combined 1

		# Max number of RX slots
		ethtool -G $IF rx 4096

		# Max number of TX slots
		ethtool -G $IF tx 4096

		# Disabling offloads
		ethtool -K $IF sg off tso off gso off gro off > /dev/null 2>&1

		# Disabling VLAN stripping
		ethtool -K $IF rxvlan off

		# Flow Control automatically disabled by the driver (no need to use the following commands)
		#ethtool -A $IF autoneg off
		#ethtool -A $IF rx off
		#ethtool -A $IF tx off
		#ethtool -s $IF speed 10000

		# Enable n-tuple hw filters
		#ethtool -K $IF ntuple on

		ifconfig $IF up
		sleep 1
		bash ../scripts/set_irq_affinity $IF
	fi
done

HUGEPAGES_NUM=1024
HUGEPAGES_PATH=/dev/hugepages
sync && echo 3 > /proc/sys/vm/drop_caches
echo $HUGEPAGES_NUM > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
if [ `cat /proc/mounts | grep hugetlbfs | grep $HUGEPAGES_PATH | wc -l` -eq 0 ]; then
	if [ ! -d $HUGEPAGES_PATH ]; then
		mkdir $HUGEPAGES_PATH
	fi
	mount -t hugetlbfs nodev $HUGEPAGES_PATH
fi
HUGEPAGES_AVAIL=$(grep HugePages_Total /sys/devices/system/node/node0/meminfo | cut -d ':' -f 2|sed 's/ //g')
if [ $HUGEPAGES_AVAIL -ne $HUGEPAGES_NUM ]; then 
	printf "Warning: %s hugepages available, %s requested\n" "$HUGEPAGES_AVAIL" "$HUGEPAGES_NUM"
fi

