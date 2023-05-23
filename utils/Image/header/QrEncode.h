#pragma once
#include <string>
#ifndef _QR_ENCODE_
#define _QR_ENCODE_

#include "Util.h"
#include "Matrix.h"
#include "Image.h"

NAME_SPACE_START(myUtil)

class QREncode{
public:
    QREncode(const string& imageFilePath):_imageFilePath(imageFilePath){}
    ~QREncode(){}
    string encoding(const string& encodeData);
    string decoding(const string& outputPath);
    Matrix<RGB> getFileRGB();
private:
    string _imageFilePath;
};

NAME_SPACE_END()

#endif //!_QR_ENCODE_