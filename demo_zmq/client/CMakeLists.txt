include_directories(
        ../common
)
add_executable(demo_zmq_client demo_zmq_client.c)
target_link_libraries(demo_zmq_client zmq pthread)