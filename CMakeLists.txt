cmake_minimum_required(VERSION 3.0.0)
project(MAIN VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)
set(UTILS_PATH ${PROJECT_SOURCE_DIR}/utils)

add_subdirectory(utils/Matrix)
add_subdirectory(utils/Log)
add_subdirectory(utils/Reflex)
# add_subdirectory(utils/JsonSerialize)
add_subdirectory(utils/Util)
add_subdirectory(utils/Singleton)

add_executable(MAIN main.cpp)

include_directories(${UTILS_PATH}/Matrix/header)
include_directories(${UTILS_PATH}/Log/header)
include_directories(${UTILS_PATH}/Util/header)
# include_directories(${UTILS_PATH}/JsonSerialize/header)
include_directories(${UTILS_PATH}/Reflex/header)
include_directories(${UTILS_PATH}/Singleton/header)

target_link_libraries(MAIN Matrix_static logger_static Util_static)
target_link_libraries(MAIN Singleton_static)
# target_link_libraries(MAIN Reflex_static)
# target_link_libraries(MAIN JsonSerialize_static)

enable_testing()

add_test(NAME MAIN_TEST COMMAND MAIN)