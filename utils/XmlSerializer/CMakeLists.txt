cmake_minimum_required(VERSION 3.5.0)
project(XmlSerializer CXX)
file(GLOB SOURCE_FILE src/*.cpp)

add_library(xml_static STATIC ${SOURCE_FILE})

target_include_directories(xml_static
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include)
target_link_libraries(xml_static Util_static)

add_executable(XmlTest test/xml_test.cpp)
target_link_libraries(XmlTest xml_static)
add_test(Xml_Test XmlTest)