cmake_minimum_required(VERSION 3.5.0)
project(Video CXX)
set(UTIL_PATH "..")
file(GLOB SOURCE_FILE src/video.cpp)

add_library(video_static STATIC ${SOURCE_FILE})

include_directories(header)
include_directories(${UTIL_PATH}/Util/header)
target_include_directories(video_static PUBLIC header)
target_link_libraries(video_static Util_static)