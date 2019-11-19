# find pf_ring
set(PF_RING_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/pf_ring/PF_RING-7.0.0")
find_path(PF_RING_INCLUDE_DIR
        NAMES pfring.h
        HINTS ${PF_RING_ROOT_DIR}/userland/lib
        )
find_library(PF_RING_LIBRARY
        NAMES pfring
        HINTS ${CMAKE_CURRENT_LIST_DIR}
        )
message("PF_RING_INCLUDE_DIR=${PF_RING_INCLUDE_DIR}")
message("PF_RING_LIBRARY=${PF_RING_LIBRARY}")

# include headers
include_directories(
        ${PF_RING_INCLUDE_DIR}
)

add_executable(demo_pfring demo_pfring.c)

target_link_libraries(demo_pfring ${PF_RING_LIBRARY} dl pthread)

add_subdirectory(pf_ring)
