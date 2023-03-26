cmake_minimum_required(VERSION 3.0.0)
project(Server VERSION 0.1.0)
# set(LIB "F:/Visual Studio practice/vscode/newdemo/build")
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
set(UTIL_PATH "..")
set(LINK_DIR ../../lib) 
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(header)
include_directories(${UTIL_PATH}/Util/header)
include_directories(${UTIL_PATH}/Reflex/header)
include_directories(${UTIL_PATH}/Singleton/header)

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