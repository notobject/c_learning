add_executable(demo_protobuf demo_protobuf.c demo.pb-c.c)
target_link_libraries(demo_protobuf protobuf-c)