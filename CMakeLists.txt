cmake_minimum_required(VERSION 3.5)
project(mqproxy)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR
    NAMES zmq.hpp
    PATHS ${PC_ZeroMQ_INCLUDE_DIRS})

find_library(ZeroMQ_LIBRARY
    NAMES zmq
    PATHS ${PC_ZeroMQ_LIBRARY_DIRS})

set(SOURCE_FILES mqproxy.cpp)
add_executable(mqproxy ${SOURCE_FILES})

target_include_directories(mqproxy PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(mqproxy PUBLIC ${ZeroMQ_LIBRARY})
