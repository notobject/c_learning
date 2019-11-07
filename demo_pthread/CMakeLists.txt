add_executable(demo_pthread demo_pthread.c)
target_link_libraries(demo_pthread pthread)

add_subdirectory(thread_pool)