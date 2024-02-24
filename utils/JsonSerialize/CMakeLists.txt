cmake_minimum_required(VERSION 3.5.0)
project(JsonSerialize VERSION 0.1.0)
# set(LIB "F:/Visual Studio practice/vscode/newdemo/build")
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
set(UTIL_PATH "..")
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(header)
include_directories(${UTIL_PATH}/Util/header
                    ${UTIL_PATH}/Reflex/header
                    ${UTIL_PATH}/Singleton/header)

add_library(JsonSerialize SHARED ${SOURCE_FILE}
        header/JsonException.h)
#add_library(JsonSerialize_static STATIC ${SOURCE_FILE})
add_library(JsonSerialize_static STATIC ${SOURCE_FILE})

#target_link_libraries(JsonSerialize_static
#        Util_static Reflex_static Singleton_static)
target_link_libraries(JsonSerialize_static Util_static)