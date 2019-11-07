include_directories(
        ../common
)
add_executable(demo_zmq_server demo_zmq_server.c)
target_link_libraries(demo_zmq_server zmq)