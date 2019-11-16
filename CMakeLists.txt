cmake_minimum_required(VERSION 2.8.4)
project(c_learning C CXX)
set(CMAKE_C_STANDARD 99)


# include headers
include_directories(
        /usr/local/include
        /usr/include
)

add_executable(c_learning main.c)

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
add_subdirectory(demo_base64)
add_subdirectory(demo_nids)
# add_subdirectory(demo_pfring)
add_subdirectory(demo_mempool)
