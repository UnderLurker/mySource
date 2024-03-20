cmake_minimum_required(VERSION 3.5.0)
project(logger CXX)
file(GLOB SOURCE_FILE ./src/logger.cpp)

#include_directories(include)
## add_library(Matrix SHARED ${SOURCE_FILE})
## target_include_directories(Matrix PUBLIC header)
## install(TARGETS Matrix LIBRARY DESTINATION lib)
#add_library(logger_static STATIC ${SOURCE_FILE})
#
#include_directories(${UTILS_PATH}/Util/include)
#target_include_directories(logger_static PUBLIC header)
#target_link_libraries(logger_static Util_static)
