message("Compile pf_ring")

set(PF_RING_ROOT_DIR PF_RING-7.0.0)
find_path(PF_RING_INCLUDE_DIR
        NAMES pfring.h
        HINTS ${PF_RING_ROOT_DIR}/userland/lib ${PF_RING_ROOT_DIR}/include
        )

find_library(PF_RING_LIBRARY
        NAMES pfring
        HINTS ${PF_RING_ROOT_DIR}/userland/lib/libs ${PF_RING_ROOT_DIR}/lib
        )
message("PF_RING_ROOT_DIR=${PF_RING_ROOT_DIR}")
message("PF_RING_INCLUDE_DIR=${PF_RING_INCLUDE_DIR}")
message("PF_RING_LIBRARY=${PF_RING_LIBRARY}")
# kernel

