cmake_minimum_required(VERSION 3.5.0)
project(DataCompress CXX)
file(GLOB SOURCE_FILE src/*.cpp)

add_library(dataCompress_static STATIC ${SOURCE_FILE})

target_include_directories(dataCompress_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(dataCompress_static Util_static)

add_executable(CompressTest test/Compress_test.cpp)
target_link_libraries(CompressTest dataCompress_static)
add_test(Compress_test CompressTest)