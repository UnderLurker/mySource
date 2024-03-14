cmake_minimum_required(VERSION 3.5.0)
project(Reflex VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(include)
include_directories(${UTILS_PATH}/Util/include)
include_directories(${UTILS_PATH}/Singleton/include)

add_executable(ReflexTest test/reflex_test.cpp)
add_test(Reflex_Test ReflexTest)
