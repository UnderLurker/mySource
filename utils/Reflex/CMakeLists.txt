cmake_minimum_required(VERSION 3.5.0)
project(Reflex VERSION 0.1.0)
# set(LIB "F:/Visual Studio practice/vscode/newdemo/build")
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
set(UTIL_PATH "..")
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(header)
include_directories(${UTIL_PATH}/Util/header)
include_directories(${UTIL_PATH}/Singleton/header)

# add_library(Reflex SHARED ${SOURCE_FILE})
add_library(Reflex_static STATIC ${SOURCE_FILE})

target_include_directories(Reflex_static PUBLIC header)

target_link_libraries(Reflex_static Util_static)
target_link_libraries(Reflex_static Singleton_static)

# install(TARGETS Reflex LIBRARY DESTINATION lib)
install(TARGETS Reflex_static ARCHIVE DESTINATION lib)
install(FILES include/*.h DESTINATION lib)
