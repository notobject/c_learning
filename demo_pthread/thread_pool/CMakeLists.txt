add_definitions("-DDEBUG_THREAD_POOL")

add_executable(test_thread_pool test_thread_pool.c)
target_link_libraries(test_thread_pool pthread)