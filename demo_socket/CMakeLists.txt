add_executable(demo_socket demo_socket.c)
target_link_libraries(demo_socket pthread)

add_subdirectory(select)
add_subdirectory(poll)
add_subdirectory(epoll)