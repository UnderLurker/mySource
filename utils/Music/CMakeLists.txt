cmake_minimum_required(VERSION 3.5.0)
project(Music VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)
file(GLOB M4A_FILE ./src/m4a/*.cpp)

add_library(Music_static STATIC ${SOURCE_FILE} ${M4A_FILE})
target_include_directories(Music_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(Music_static Util_static)

add_library(Music_shared SHARED ${SOURCE_FILE} ${M4A_FILE})
target_include_directories(Music_shared
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(Music_shared Util_static)

add_executable(MusicTest test/Music_test.cpp)
target_link_libraries(MusicTest Music_shared)
target_include_directories(MusicTest PUBLIC ${UTILS_PATH}/Log/include)
add_test(Music_Test MusicTest)

#add_custom_command(TARGET MusicTest POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        $<TARGET_FILE_DIR:uv>/libuv.dll
#        $<TARGET_FILE_DIR:MusicTest>/libuv.dll
#        COMMENT "copy libuv.dll MusicTest")