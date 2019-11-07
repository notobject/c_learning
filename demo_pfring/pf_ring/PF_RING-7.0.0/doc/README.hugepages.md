# Hugepages support in PF_RING ZC

Hugepages are used by ZC for buffers allocations. This document describes 
how to enable them into your system. 

## For the impatient
```
# 1024 * 2048kB = 2 GB
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
mount -t hugetlbfs nodev /dev/hugepages
```

_Warning: If you accidentally exceed the amount of available RAM, the kernel will round down to the highest number of hugepages possible and grind your system to a halt._

## The whole story
Linux typically use memory pages of 4 KBytes, but provides an explicit 
interface to allocate pages with bigger size called hugepages. It is up 
to developers/administrators to decide when they have to be used. 

Hugepages advantages:

- large amounts of physical memory can be reserved for memory allocation, 
  that otherwise would fail especially when physically contiguous memory 
  is required.

- reduced overhead: as the TLB (Translation Lookaside Buffer) contains 
  per-page virtual to physical address mappings, using a large amount of
  memory with the default page size leads to processing overhead for 
  managing the TLB entries.

The default hugepage size is usually 2 MBytes. The hugepage size can be 
found in /proc/meminfo:

```
cat /proc/meminfo | grep Hugepagesize
Hugepagesize: 2048 kB 
```

Hugepages can be dynamically reserved with:

```
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
```

The above pages are allocated by the system without node affinity. If
you want to force allocation on a specific NUMA node you have to do:

```
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
```

It is possible to change the default hugepages size and reserve large 
amounts of memory at boot time using the following kernel options:

```
  default_hugepagesz=1G hugepagesz=1G hugepages=4
```

If this commands returns a non-empty string, 2MB pages are supported:

```
cat /proc/cpuinfo | grep pse
```

If this commands returns a non-empty string, 1GB pages are supported:

```
cat /proc/cpuinfo | grep pdpe1gb
```

In order to make the hugepages available for use, Linux provides a 
RAM-based filesystem called "hugetlbfs" that have to be mounted with:

```
mount -t hugetlbfs none /dev/hugepages
```

With no options the default hugepage size is used. To use a different
size it is possible to specify the "pagesize=" option.
In order to control the maximum amount of memory bound to a mount point
it is possible to specify the "size=" option (size is rounded down to 
the nearest hugepage size). 
Example:

```
mount -t hugetlbfs -o pagesize=1G,size=2G none /dev/hugepages
```

It is possible to see what pages are currently in use using the
following command:

```
cat /sys/devices/system/node/node*/meminfo | grep Huge
Node 0 HugePages_Total:  1024
Node 0 HugePages_Free:   1024
Node 0 HugePages_Surp:      0
```

## Possible errors:
1. "error mmap'ing hugepage xxx: Resource temporarily unavailable"

Causes:
- You might not have permissions to mmap hugepages.

- Increase the "max locked memory" ulimit (see ulimit -l) to accomodate 
  the memory needed for buffers and metadata. You can manually increase 
  the ulimit value:

```
ulimit -l 4194304
```

- Increase the value in /proc/sys/kernel/shmmax. Please note that on 
  32bit systems it will wrap around 4 GBytes.

```
echo 2147483648 > /proc/sys/kernel/shmmax
```

