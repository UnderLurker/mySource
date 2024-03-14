cmake_minimum_required(VERSION 3.5.0)
project(Thread CXX)
file(GLOB SOURCE_FILE ./src/*.cpp)

add_library(Thread_static STATIC ${SOURCE_FILE})

target_include_directories(Thread_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include
        PUBLIC ${UTILS_PATH}/Singleton/include)
target_link_libraries(Thread_static Util_static Singleton_static)

add_executable(ThreadTest test/Thread_test.cpp)
target_link_libraries(ThreadTest Thread_static)
add_test(Thread_test ThreadTest)
