cmake_minimum_required(VERSION 3.5.0)
project(Server VERSION 0.1.0)
# set(LIB "F:/Visual Studio practice/vscode/newdemo/build")
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
set(LINK_DIR ../../lib)
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(header)
include_directories(${UTILS_PATH}/Util/include)
include_directories(${UTILS_PATH}/Reflex/include)
include_directories(${UTILS_PATH}/Singleton/include)

# add_library(Server SHARED ${SOURCE_FILE})
add_library(Server_static STATIC ${SOURCE_FILE})

target_include_directories(Server_static PUBLIC header)

target_link_libraries(Server_static Util_static)
target_link_libraries(Server_static Reflex_static)
target_link_libraries(Server_static Singleton_static)

link_directories(${LINK_DIR})
link_libraries(WS2_32)
# install(TARGETS Server LIBRARY DESTINATION lib)
# install(TARGETS Server_static ARCHIVE DESTINATION lib)
# install(FILES include/*.h DESTINATION lib)

add_executable(ServerTest test/server_test.cpp)
target_link_libraries(ServerTest Server_static)
add_test(Server_Test ServerTest)