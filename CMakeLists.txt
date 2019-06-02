# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (rdma_demo)

# Compiler options
set (CMAKE_C_FLAGS "-Wall -Wno-deprecated-declarations -Wsign-compare")
set (CMAKE_CXX_FLAGS "-g ${CMAKE_C_FLAGS} -std=c++11 -Wall -O0 -DTHREAD -D_FILE_OFFSET_BITS=64 -libverbs -lfuse")

option (SANITIZE "Turn on sanitization" OFF)
if (SANITIZE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

# Incs and Libs
set(INCLUDE_BASE ${PROJECT_SOURCE_DIR}/include)
include_directories("./rdma_demo/include" ${INCLUDE_BASE})
link_libraries(${CRYPTO_LIBRARIES} ${CMAKE_DL_LIBS})

# Source file define
set(SRC_NET ${PROJECT_SOURCE_DIR}/src/net)

# Build library
file(GLOB_RECURSE SRC_NET_LIB ${SRC_NET}/*.cpp)
add_library(net SHARED ${SRC_NET_LIB})
link_libraries(net pthread)


#Build server
add_executable(server ${PROJECT_SOURCE_DIR}/src/server/teser.cpp)
#target_link_libraries(net Threads::Threads)

#Build client
add_executable(client ${PROJECT_SOURCE_DIR}/src/client/tecli.cpp)
