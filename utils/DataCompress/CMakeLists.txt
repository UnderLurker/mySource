cmake_minimum_required(VERSION 3.5.0)
project(DataCompress CXX)
set(UTIL_PATH "..")
#file(GLOB SOURCE_FILE src/compress.cpp)

add_library(dataCompress_static STATIC src/compress.cpp)

include_directories(header)
include_directories(${UTIL_PATH}/Util/header)
target_include_directories(dataCompress_static PUBLIC header)
target_link_libraries(dataCompress_static Util_static)
