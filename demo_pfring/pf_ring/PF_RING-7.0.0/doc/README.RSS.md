# RSS (Receive Side Scaling)
Almost all Intel (and other vendors) NICs have RSS support, this means they
are able to hash packets in hardware in order to distribute the load across 
multiple RX queues.

In order to configure the number of queues, you can use the RSS parameter at 
insmod time (if you are installing PF_RING ZC drivers from packages you can 
use the configuration file as explained in README.apt_rpm_packages), passing 
a comma-separated list (one per interface) of numbers (number of queues per
interface). Examples:

Enable as many queues as the number of processors, per interface:
``` 
insmod ixgbe.ko RSS=0,0,0,0
``` 

Enable 4 RX queues per interface:
``` 
insmod ixgbe.ko RSS=4,4,4,4
``` 

Disable multiqueue (1 RX queue per interface):
``` 
insmod ixgbe.ko RSS=1,1,1,1
``` 

Alternatively it is possible to configure the number of RX queues at runtime
using ethtool:

``` 
ethtool --set-channels <if> combined 1
``` 

RSS distributes the load across the specified number of RX queues based on an 
hash function which is IP-based (or IP/Port-based in case of TCP), in combination 
with an indirection table: queue = indirection_table[hash(packet)]
You can see the content of the indirection table with:

``` 
ethtool -x <if>
``` 

It is possible to configure the indirection table by simply applying weights 
to each RX queue using ethtool. For instance if we want all traffic to go to
queue 0, and we configured the card with 4 RX queues, we can use the command
below:

``` 
ethtool -X <if> weight 1 0 0 0
``` 

## Naming convention

In order to open a specific interface queue, you have to specify the queue ID
using the "@<ID>" suffix. Example:

``` 
pfcount -i zc:eth1@0
``` 

Please note that if you configure an interface with multiple RSS queues, and
you open it using ZC with zc:eth1, this is the same as opening zc:eth1@0.
This does not apply in standard kernel mode, where kernel abstracts the
interface and capturing from eth1 means capturing from all the queues. This
happens because ZC is a kernel-bypass technology, thus there is no abstraction,
and the application directly opens an interface queue, which corresponds to the
full interface only when RSS=1.

