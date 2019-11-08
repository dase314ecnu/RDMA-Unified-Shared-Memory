# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (rdma_demo)

# Compiler options
set (CMAKE_C_FLAGS "-Wall -Wno-deprecated-declarations -Wsign-compare")
set (CMAKE_CXX_FLAGS "-g ${CMAKE_C_FLAGS} -std=c++11 -pthread -Wall -O0 -DTHREAD -D_FILE_OFFSET_BITS=64 -libverbs -lfuse")

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
set(SRC_INDEX ${PROJECT_SOURCE_DIR}/src/index)
set(SRC_CLIENT ${PROJECT_SOURCE_DIR}/src/client)
set(SRC_SERVER ${PROJECT_SOURCE_DIR}/src/server)

# Build library
file(GLOB_RECURSE SRC_NET_LIB ${SRC_NET}/*.cpp)
add_library(net SHARED ${SRC_NET_LIB})
link_libraries(net)

file(GLOB_RECURSE SRC_INDEX_LIB ${SRC_INDEX}/*.cpp)
add_library(index SHARED ${SRC_INDEX_LIB})
link_libraries(index)



#Build server
add_executable(server ${PROJECT_SOURCE_DIR}/src/server/teser.cpp)
#target_link_libraries(net Threads::Threads)

#Build client
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE NET_LIB ${SRC_NET}/*.cpp)
file(GLOB_RECURSE INDEX_LIB ${SRC_INDEX}/*.cpp)
file(GLOB_RECURSE CLIENT_LIB ${SRC_CLIENT}/*.cpp)
file(GLOB_RECURSE SERVER_LIB ${SRC_SERVER}/*.cpp)

add_library(RDMAClientStatic ${NET_LIB} ${INDEX_LIB} ${CLIENT_LIB} ${SERVER_LIB})
