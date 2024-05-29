cmake_minimum_required(VERSION 3.5.0)
project(ULGui VERSION 0.1.0)
#baseGraphics
file(GLOB BASE_SOURCE ./base/src/*.cpp)

add_library(BaseGraphics SHARED ${BASE_SOURCE})
target_include_directories(BaseGraphics
        PUBLIC include
        PUBLIC base/include
        PUBLIC ${THIRD_PART}/glm)
target_link_libraries(BaseGraphics
        opengl32)

#events
file(GLOB EVENT_SOURCE ./events/src/*.cpp)

add_library(Event SHARED ${EVENT_SOURCE})
target_include_directories(Event
        PUBLIC events/include)

#window
file(GLOB WINDOW_SOURCE ./src/*.cpp)

add_library(Window SHARED ${WINDOW_SOURCE})
target_include_directories(Window
        PUBLIC include
        PUBLIC base/include
        PUBLIC events/include
        PUBLIC components/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include)
target_link_libraries(Window
        BaseGraphics
        opengl32
        Event
        ${OpenGL_DIR}/lib/GLFW/libglfw3.a)

#components
file(GLOB COMPONENTS_SOURCE ./components/src/*.cpp)

add_library(Components SHARED ${COMPONENTS_SOURCE})
target_include_directories(Components
        PUBLIC include
        PUBLIC base/include
        PUBLIC components/include
        PUBLIC ${THIRD_PART}/glm
        PUBLIC ${THIRD_PART}/std
        PUBLIC ${THIRD_PART}/glfw/include)
target_link_libraries(Components Window Event)

#graphicsTest
add_executable(windowTest test/ULGui_test.cpp ${OpenGL_DIR}/src/glad.c)
target_include_directories(windowTest
        PUBLIC include
        PUBLIC ../include
        PUBLIC base/include
        PUBLIC events/include
        PUBLIC components/include)
target_link_libraries(windowTest
        Window
        opengl32
        BaseGraphics
        Components
        Event
        ${OpenGL_DIR}/lib/GLFW/libglfw3.a)
add_test(Widow_test windowTest)