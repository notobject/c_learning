include_directories(include)

set(SRC_FILES mempool.c mphash.c)

add_library(mempool ${SRC_FILES})