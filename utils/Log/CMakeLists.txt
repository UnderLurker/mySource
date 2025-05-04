cmake_minimum_required(VERSION 3.5.0)
project(logger CXX)
file(GLOB SOURCE_FILE ./src/logger.cpp)
# LoggerTest
add_executable(LoggerTest test/log_test.cpp)
target_link_libraries(LoggerTest Util_static)
target_include_directories(LoggerTest
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Log/include
        PUBLIC ${UTILS_PATH}/Util/include)
add_test(Logger_Test LoggerTest)
# LogBase
file(GLOB LOG_BASE_SOURCE_FILE ./base/*.cpp)

# LogClient_shared
file(GLOB LOG_CLIENT_SOURCE_FILE ./client/src/*.cpp)
add_library(LogClient_shared SHARED ${LOG_CLIENT_SOURCE_FILE} ${LOG_BASE_SOURCE_FILE})
target_link_libraries(LogClient_shared uv)
target_include_directories(LogClient_shared
        PUBLIC .
        PUBLIC ${THIRD_PART}/libuv/include)

add_library(LogClient_static SHARED ${LOG_CLIENT_SOURCE_FILE} ${LOG_BASE_SOURCE_FILE})
target_link_libraries(LogClient_static uv)
target_include_directories(LogClient_static
        PUBLIC .
        PUBLIC ${THIRD_PART}/libuv/include)

# LogServer_shared
file(GLOB LOG_SERVER_SOURCE_FILE ./server/src/*.cpp)
add_library(LogServer_shared SHARED ${LOG_SERVER_SOURCE_FILE} ${LOG_BASE_SOURCE_FILE})
target_link_libraries(LogServer_shared uv)
target_include_directories(LogServer_shared
        PUBLIC .
        PUBLIC ${THIRD_PART}/libuv/include)

add_library(LogServer_static SHARED ${LOG_SERVER_SOURCE_FILE} ${LOG_BASE_SOURCE_FILE})
target_link_libraries(LogServer_static uv)
target_include_directories(LogServer_static
        PUBLIC .
        PUBLIC ${THIRD_PART}/libuv/include)

# LogClientTest
add_executable(LogClientTest test/log_client_test.cpp)
target_link_libraries(LogClientTest LogClient_shared)
target_include_directories(LogClientTest PUBLIC .)
add_test(LogClient_Test LogClientTest)

#add_custom_command(TARGET LogClientTest POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#    $<TARGET_FILE_DIR:uv>/libuv.dll
#    $<TARGET_FILE_DIR:LogClientTest>/libuv.dll
#    COMMENT "copy libuv.dll LogClientTest")

# LogServerTest
add_executable(LogServerTest test/log_server_test.cpp)
target_link_libraries(LogServerTest LogServer_shared)
target_include_directories(LogServerTest PUBLIC .)
add_test(LogServer_Test LogServerTest)

add_custom_command(TARGET LogServerTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE_DIR:uv>/libuv.dll
        $<TARGET_FILE_DIR:LogServerTest>/libuv.dll
        COMMENT "copy libuv.dll LogServerTest")