cmake_minimum_required(VERSION 3.5.0)
project(Singleton VERSION 0.1.0)
set(CMAKE_INSTALL_PREFIX "F:/Visual-Studio-practice/vscode/mySource/build")
file(GLOB SOURCE_FILE ./src/*.cpp)

include_directories(include)
include_directories(${UTILS_PATH}/Util/include)
add_library(Singleton SHARED ${SOURCE_FILE})
add_library(Singleton_static STATIC ${SOURCE_FILE})
# install(TARGETS Singleton LIBRARY DESTINATION lib)
install(TARGETS Singleton_static ARCHIVE DESTINATION lib)
install(FILES include/*.h DESTINATION lib)

target_link_libraries(Singleton_static Util_static)