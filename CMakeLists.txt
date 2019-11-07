cmake_minimum_required(VERSION 2.8.4)
project(c_learning C CXX)
set(CMAKE_C_STANDARD 99)

add_subdirectory(libs/mempool)
add_subdirectory(libs/utils)
add_subdirectory(libs/nids)

# find pf_ring
set(PF_RING_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/pf_ring/PF_RING-7.0.0")
find_path(PF_RING_INCLUDE_DIR
        NAMES pfring.h
        HINTS ${PF_RING_ROOT_DIR}/userland/lib
        )
find_library(PF_RING_LIBRARY
        NAMES pfring
        HINTS ${CMAKE_CURRENT_LIST_DIR}/libs/lib/pf_ring
        )
message("PF_RING_INCLUDE_DIR=${PF_RING_INCLUDE_DIR}")
message("PF_RING_LIBRARY=${PF_RING_LIBRARY}")

# include headers
include_directories(
        /usr/local/include
        /usr/include
        libs/mempool/include
        libs/utils/include
        libs/nids/libnids-1.24/src/
        ${PF_RING_INCLUDE_DIR}
)

aux_source_directory(demos DEMO_SRC)
add_executable(c_learning main.c ${DEMO_SRC})
target_link_libraries(c_learning mempool base64 nids ${PF_RING_LIBRARY} dl)

add_subdirectory(demo_event)
add_subdirectory(demo_pthread)
add_subdirectory(demo_magic)
add_subdirectory(demo_daemonize)
add_subdirectory(demo_atomic)
add_subdirectory(demo_TAILQ)
add_subdirectory(demo_LIST)
add_subdirectory(demo__attribute__)
add_subdirectory(demo_trik)
add_subdirectory(demo_getopt)
add_subdirectory(demo_conf)
add_subdirectory(demo_json)
add_subdirectory(demo_hash)
add_subdirectory(demo_dict)
add_subdirectory(demo_protobuf)
add_subdirectory(demo_zmq)
add_subdirectory(demo_file)
add_subdirectory(demo_datetime)
add_subdirectory(demo_socket)
add_subdirectory(demo_openssl)
add_subdirectory(demo_log)
add_subdirectory(demo_mmap)
