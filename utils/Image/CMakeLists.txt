cmake_minimum_required(VERSION 3.5.0)
project(Image VERSION 0.1.0)
file(GLOB IMAGE_SOURCE_FILE src/bmp.cpp)
file(GLOB QRCODE_SOURCE_FILE ./src/QrEncode.cpp)

add_library(Image_static STATIC ${IMAGE_SOURCE_FILE}
        ./src/jpeg.cpp
        ./src/png.cpp)
target_link_libraries(Image_static Util_static Matrix_static)
target_include_directories(Image_static PUBLIC
        include
        ${UTILS_PATH}/Util/include
        ${UTILS_PATH}/Matrix/include)

add_executable(ImageTest test/Image_test.cpp)
target_link_libraries(ImageTest Image_static)
add_test(Image_Test ImageTest)

add_library(QRCode_static STATIC ${QRCODE_SOURCE_FILE})
target_link_libraries(QRCode_static Image_static Util_static Matrix_static)
target_include_directories(QRCode_static PUBLIC
        include
        ${UTILS_PATH}/Util/include
        ${UTILS_PATH}/Matrix/include)

add_executable(QrCodeTest test/QrCode_test.cpp)
target_link_libraries(QrCodeTest QRCode_static)
add_test(QrCode_Test QrCodeTest)