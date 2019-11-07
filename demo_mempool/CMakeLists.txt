include_directories(include)

set(SRC_FILES demo_mempool.c mempool.c mphash.c)

add_executable(demo_mempool ${SRC_FILES})