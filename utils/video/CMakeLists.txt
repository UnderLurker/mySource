cmake_minimum_required(VERSION 3.5.0)
project(Video CXX)
file(GLOB SOURCE_FILE src/video.cpp)

add_library(video_static STATIC ${SOURCE_FILE})

target_include_directories(video_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(video_static Util_static)

add_executable(VideoTest test/Video_test.cpp)
target_link_libraries(VideoTest video_static)
add_test(Video_test VideoTest)