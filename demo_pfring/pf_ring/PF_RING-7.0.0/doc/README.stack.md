# Linux TCP/IP Stack injection ("stack") module

This module can be used to inject/capture packets to/from the Linux TCP/IP Stack,
simulating the arrival/sending of those packets on an interface. The application
has to open a ring by using as device name "stack:ethX" where ethX is the interface
bound to the packets injected into the stack. In order to inject a packet to the
stack pfring_send() has to be used, in order to capture outgoing packets
pfring_recv() has to be used.

You can test the stack module with all PF_RING apps. Example:

```
pfcount -i stack:eth3
```
Read packets sent from the IP stack towards the eth3 interface

```
pfsend -i stack:eth3
```
Forge packets and send them to the IP stack as if they were received from the eth3 interface
