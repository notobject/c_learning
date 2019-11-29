set(TARGET demp_pcap)
aux_source_directory(./ SOURCES)
set(DEPS_LIB )

# libpcap
find_package(FindPCAP)
if(PCAP_FOUND)
    include_directories(${PCAP_INCLUDE_DIR})
    list(APPEND DEPS_LIB ${PCAP_LIBRARY})
else(PCAP_FOUND)
    message(FATAL_ERROR "libpcap not found!")
endif(PCAP_FOUND)

list(APPEND DEPS_LIB nids)
include_directories(${PROJECT_SOURCE_DIR}/demo_nids/nids/libnids-1.24/src)

list(APPEND DEPS_LIB net)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${DEPS_LIB})