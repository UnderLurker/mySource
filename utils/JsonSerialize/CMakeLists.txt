cmake_minimum_required(VERSION 3.5.0)
project(JsonSerialize VERSION 0.1.0)
file(GLOB SOURCE_FILE ./src/*.cpp)

#add_library(JsonSerialize SHARED ${SOURCE_FILE}
#        include/JsonException.h)
#add_library(JsonSerialize_static STATIC ${SOURCE_FILE})

#target_link_libraries(JsonSerialize_static
#        Util_static Reflex_static Singleton_static)
#target_link_libraries(JsonSerialize_static Util_static)

#add_executable(JsonTest ${UTILS_TEST_PATH}/json_test.cpp)
#target_link_libraries(JsonTest JsonSerialize_static)
#add_test(Json_Test JsonTest)

add_executable(JsonParseTest test/json_test.cpp)
target_link_libraries(JsonParseTest
    gtest)
if (linux)
    target_link_libraries(JsonParseTest pthread)
endif ()
target_include_directories(JsonParseTest
        PUBLIC include
        PUBLIC ${UTILS_PATH}/Util/include
        PUBLIC ${UTILS_PATH}/Reflex/include
        PUBLIC ${UTILS_PATH}/Singleton/include
        PUBLIC ${THIRD_PART}/googletest/googletest/include)

add_test(JsonParse_Test JsonTest)