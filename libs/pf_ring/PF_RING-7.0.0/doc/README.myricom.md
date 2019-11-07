# Myricom support in PF_RING

## Prerequisite
Myricom SNF v3 or v4 installed.

PF_RING has native support for Myricom adapters, the Myricom library needs 
to be installed (under /opt/snf) in order to enable the Myricom module at
runtime.

## Installation
In order to get up and running with Myricom just run the following commands.

Firmware update (usually not needed):

```
tar xvf phx-tools-*.tar
cd phx-tools
./bin/phx-replace-eeprom ./fw-8D-Q-1.4.1.rpd 
reboot
```

Myricom SNF library installation:

```
tar xvf myri_snf-*.x86_64.tar
mv myri_snf-*.x86_64 /opt/snf

cd /opt/snf
./sbin/rebuild.sh
```

Myricom service start:

```
./sbin/myri_start_stop start
```

If you are compiling PF_RING from sources:

```

cd PF_RING/kernel
make
sudo insmod pf_ring.ko
cd ../userland/lib
./configure
make
cd ../libpcap
./configure
make
cd ../examples
make

sudo ./pfcount -i myri:0
```

If you are installing from repository:

```
pfcount -i myri:0
```

Please note that in order to open port 0 from the Myricom adapter 
you should specify "myri:0"

Multi-process traffic duplication example:
```
pfcount -i myri:A1P0
pfcount -i myri:A2P0
```
Where A1 means APP ID 1, P0 means Port 0
Or
```
SNF_APP_ID=1 pfcount -i myri:0
SNF_APP_ID=2 pfcount -i myri:0
```

Multi-process traffic sharing (RSS) example:
```
pfcount -i myri:A1R2P0@0
pfcount -i myri:A1R2P0@1
```
Where A1 means APP ID 1, R2 means RSS with 2 Rings, P0 means Port 0, @0 means Ring 0
Or
```
SNF_APP_ID=1 SNF_NUM_RINGS=2 SNF_RING_ID=0 pfcount -i myri:0
SNF_APP_ID=1 SNF_NUM_RINGS=2 SNF_RING_ID=1 pfcount -i myri:0
```

Ports aggregation example:
```
pfcount -i myri:0,1
```
Or
```
SNF_FLAGS=0x2 ./pfcount -i myri:3
```
Where 3 is a mask, including port 0 and 1.

