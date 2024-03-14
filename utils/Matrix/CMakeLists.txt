cmake_minimum_required(VERSION 3.5.0)
project(Matrix CXX)
file(GLOB SOURCE_FILE ./src/Matrix.cpp)

add_executable(MatrixTest test/Matrix_test.cpp)
target_include_directories(MatrixTest PUBLIC include)
add_test(Matrix_Test MatrixTest)
