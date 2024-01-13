cmake_minimum_required(VERSION 3.5.0)
project(Util VERSION 0.1.0)
# set(LIB "F:/Visual Studio practice/vscode/newdemo/build")
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(header)
include_directories(src)
add_library(Util SHARED
        src/Util.cpp
        src/Ini.cpp)
add_library(Util_static STATIC
        src/Util.cpp
        src/Ini.cpp
        src/MD5.cpp)
install(TARGETS Util LIBRARY DESTINATION lib)
install(TARGETS Util_static ARCHIVE DESTINATION lib)
install(FILES include/*.h DESTINATION lib)
