cmake_minimum_required(VERSION 3.5.0)
project(Util VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)

add_library(Util_static STATIC
        src/Util.cpp
        src/Ini.cpp
        src/MD5.cpp)
target_include_directories(Util_static PUBLIC include
        PUBLIC src
        PUBLIC ${UTILS_PATH}/Log
        PUBLIC ${THIRD_PART}/googletest/googletest/include)

add_executable(UtilTest test/Util_test.cpp)
target_link_libraries(UtilTest Util_static gtest)
add_test(Util_Test UtilTest)