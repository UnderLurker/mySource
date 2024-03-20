cmake_minimum_required(VERSION 3.5.0)
project(Music VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)

add_library(Music_static STATIC ${SOURCE_FILE})
target_include_directories(Music_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(Music_static Util_static)

add_executable(MusicTest test/Music_test.cpp)
target_link_libraries(MusicTest Music_static)
target_include_directories(MusicTest PUBLIC ${UTILS_PATH}/Log/include)
add_test(Music_Test MusicTest)
