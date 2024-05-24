cmake_minimum_required(VERSION 3.5.0)
project(graphic2D VERSION 0.1.0)
#baseGraphics
file(GLOB BASE_SOURCE ./base/src/*.cpp)

add_library(BaseGraphics SHARED ${BASE_SOURCE})
target_include_directories(BaseGraphics
        PUBLIC include
        PUBLIC base/include
        PUBLIC ${THIRD_PART}/glm)
target_link_libraries(BaseGraphics
        opengl32)

#components
file(GLOB COMPONENTS_SOURCE ./components/src/*.cpp)

add_library(components_static STATIC ${COMPONENTS_SOURCE})
target_include_directories(components_static
        PUBLIC include
        PUBLIC base/include
        PUBLIC components/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include)


#window
file(GLOB WINDOW_SOURCE ./window/*.cpp)

add_library(window_static STATIC ${WINDOW_SOURCE})
target_include_directories(window_static
        PUBLIC include
        PUBLIC base/include
        PUBLIC components/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include)

#graphicsTest
add_executable(windowTest test/graphics2D_test.cpp ${OpenGL_DIR}/src/glad.c)
target_include_directories(windowTest
        PUBLIC include
        PUBLIC ../include
        PUBLIC base/include
        PUBLIC components/include)
target_link_libraries(windowTest
        window_static
        ${OpenGL_DIR}/lib/GLFW/libglfw3.a
        opengl32
        BaseGraphics
        components_static)
add_test(Widow_test windowTest)