cmake_minimum_required(VERSION 3.5.0)
project(logger CXX)
file(GLOB SOURCE_FILE ./src/logger.cpp)

add_executable(LoggerTest test/log_test.cpp)
target_link_libraries(LoggerTest Util_static)
target_include_directories(LoggerTest
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Log/include
        PUBLIC ${UTILS_PATH}/Util/include)
add_test(Logger_Test LoggerTest)