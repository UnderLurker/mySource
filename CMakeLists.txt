cmake_minimum_required(VERSION 3.5.0)
project(MAIN VERSION 0.1.0)

# 添加opencv引用
set(OpenGL_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "" FORCE)
set(OpenCV_DIR  F:/opencv/build/x64/vc16/lib CACHE PATH "" FORCE)
set(UTILS_PATH ${PROJECT_SOURCE_DIR}/utils CACHE PATH "" FORCE)
set(UTILS_TEST_PATH ${PROJECT_SOURCE_DIR}/test CACHE PATH "" FORCE)
set(THIRD_PART ${PROJECT_SOURCE_DIR}/third_part CACHE PATH "" FORCE)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE PATH "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
# find_package(OpenCV REQUIRED)

add_subdirectory(utils/Matrix)
add_subdirectory(utils/Log)
add_subdirectory(utils/Reflex)
add_subdirectory(utils/JsonSerialize)
add_subdirectory(utils/Util)
add_subdirectory(utils/Singleton)
add_subdirectory(utils/Server)
add_subdirectory(utils/Image)
add_subdirectory(utils/Music)
add_subdirectory(utils/Thread)
add_subdirectory(utils/Video)
add_subdirectory(utils/DataCompress)
add_subdirectory(utils/Shader)
add_subdirectory(${THIRD_PART}/zlib)
add_subdirectory(graphics2D)

add_executable(MAIN main.cpp)
add_executable(OpencvTest ${UTILS_TEST_PATH}/Opencv_test.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

#target_link_libraries(MAIN Matrix_static logger_static Util_static)
#target_link_libraries(MAIN Singleton_static)
#target_link_libraries(MAIN Reflex_static)
#target_link_libraries(MAIN JsonSerialize_static)
#target_link_libraries(MAIN Server_static)

target_link_libraries(OpencvTest ${OpenCV_LIBS})

enable_testing()

add_test(Opencv_Test OpencvTest)