include_directories(
    nids/libnids-1.24/src/
    glib-2.0/
)

add_subdirectory(nids)

add_executable(demo_nids demo_nids.c)
target_link_libraries(demo_nids nids)
