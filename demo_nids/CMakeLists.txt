include_directories(
    nids/libnids-1.24/src/
)

add_subdirectory(nids)

add_executable(demo_nids demo_nids.c)
target_link_libraries(demo_nids nids)