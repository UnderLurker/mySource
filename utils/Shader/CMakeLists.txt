cmake_minimum_required(VERSION 3.5.0)
project(Shader VERSION 0.1.0)
set(UTIL_PATH "..")
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(include)
include_directories(${OpenGL_DIR}/include)
include_directories(${UTIL_PATH}/Util/header)

add_library(Shader_static STATIC ${SOURCE_FILE})

target_include_directories(Shader_static BEFORE PUBLIC ${THIRD_PART}/glm)
target_link_libraries(Shader_static Util_static)