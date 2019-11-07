message("Compile libnids")
set(CMAKE_C_FLAGS "-g -O2 -D_BSD_SOURCE -DLIBNET_VER=1 -DHAVE_ICMPHDR=1 -DHAVE_TCP_STATES=1 -DHAVE_BSD_UDPHDR=1")
set(CMAKE_C_STANDARD 99)

# 链接GLIB
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GLIB glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER})


aux_source_directory(libnids-1.24/src DIR_SRCS)
add_definitions("-D__BSD_SOURCE -D__FAVOR_BSD -DHAVE_NET_ETHERNET_H")

add_library(nids ${DIR_SRCS})
# nids 依赖 pcap,net,glib.  glib 依赖 gthread-2.0
target_link_libraries(nids pcap net gthread-2.0 ${Glib_LIBRARY})

