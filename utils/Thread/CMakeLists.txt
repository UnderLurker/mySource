cmake_minimum_required(VERSION 3.0.0)
project(Thread CXX)
set(UTIL_PATH "..")
file(GLOB SOURCE_FILE ./src/*.cpp)

add_library(Thread_static STATIC ${SOURCE_FILE})

include_directories(header)
include_directories(${UTIL_PATH}/Util/header)
include_directories(${UTIL_PATH}/Singleton/header)
target_include_directories(Thread_static PUBLIC header)
target_link_libraries(Thread_static Util_static Singleton_static)

