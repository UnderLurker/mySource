cmake_minimum_required(VERSION 3.5.0)
project(Server VERSION 0.1.0)
file(GLOB SOURCE_FILE src/*.cpp)

add_library(Server_static STATIC ${SOURCE_FILE})

target_include_directories(Server_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include
        PUBLIC ${UTILS_PATH}/Reflex/include
        PUBLIC ${UTILS_PATH}/Singleton/include)
target_link_libraries(Server_static
        Util_static
        libws2_32.a)

add_executable(ServerTest test/server_test.cpp)
#target_include_directories(ServerTest
#        PRIVATE ${UTILS_PATH}/Reflex/include
#        PRIVATE ${UTILS_PATH}/Singleton/include)
target_link_libraries(ServerTest Server_static)
add_test(Server_Test ServerTest)