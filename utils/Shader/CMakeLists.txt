cmake_minimum_required(VERSION 3.5.0)
project(Shader VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)
file(GLOB FONT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/fonts/*.ttf)
file(COPY ${FONT_FILE} DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/fonts)

function(CopyAllFile sourceDir targetDir regular)
    file(GLOB ALL_GLSL_FILE ${sourceDir}/*.*)
    set(FILTER_FILES)
    foreach(FILE ${ALL_GLSL_FILE})
        string(REGEX MATCH ${regular} RESULT ${FILE})
        if(RESULT)
            list(APPEND FILTER_FILES ${FILE})
        endif()
    endforeach()
    file(COPY ${FILTER_FILES} DESTINATION ${targetDir})
endfunction()

set(SHADER_EXTENSION "[a-zA-Z0-9_]+\\.(glsl|frag|vert)$")
set(FONT_EXTENSION "[a-zA-Z0-9_]+\\.ttf$")
CopyAllFile(${CMAKE_CURRENT_SOURCE_DIR}/shader ${CMAKE_CURRENT_BINARY_DIR}/shader ${SHADER_EXTENSION})
CopyAllFile(${CMAKE_CURRENT_SOURCE_DIR}/fonts ${CMAKE_CURRENT_BINARY_DIR}/fonts ${FONT_EXTENSION})

add_library(Shader_static STATIC ${SOURCE_FILE})
target_link_directories(Shader_static PUBLIC ${OpenGL_DIR}/lib)
target_include_directories(Shader_static
        PUBLIC include
        PUBLIC ${OpenGL_DIR}/include
        PUBLIC ${UTILS_PATH}/Util/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include
        PUBLIC ${THIRD_PART}/freetype-windows-binaries/include)
target_link_libraries(Shader_static Util_static)

add_executable(OpenGLTest test/opengl_test.cpp ${OpenGL_DIR}/src/glad.c)
target_link_libraries(OpenGLTest
        ${OpenGL_DIR}/lib/GLFW/libglfw3.a
        opengl32
        Shader_static
        "${THIRD_PART}/freetype-windows-binaries/release dll/win64/freetype.dll")
target_include_directories(OpenGLTest
        PUBLIC ${THIRD_PART}/freetype-windows-binaries/include)
add_test(OpenGL_Test OpenGLTest)