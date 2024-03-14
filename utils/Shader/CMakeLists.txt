cmake_minimum_required(VERSION 3.5.0)
project(Shader VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)

add_library(Shader_static STATIC ${SOURCE_FILE})
target_link_directories(Shader_static PUBLIC ${OpenGL_DIR}/lib)
target_include_directories(Shader_static
        PUBLIC include
        PUBLIC ${OpenGL_DIR}/include
        PUBLIC ${UTILS_PATH}/Util/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include)
target_link_libraries(Shader_static Util_static)

add_executable(OpenGLTest test/opengl_test.cpp ${OpenGL_DIR}/src/glad.c)
target_link_libraries(OpenGLTest
        ${OpenGL_DIR}/lib/GLFW/libglfw3.a
        opengl32
        Shader_static)
add_test(OpenGL_Test OpenGLTest)