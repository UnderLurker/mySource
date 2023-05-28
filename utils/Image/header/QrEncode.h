#pragma once
#include <string>
#ifndef _QR_ENCODE_
#define _QR_ENCODE_

#include "Util.h"
#include "Matrix.h"
#include "Image.h"

NAME_SPACE_START(myUtil)

enum ErrorCorrectionLevel{
    L,  //Recovers 7% of data
    M,  //Recovers 15% of data
    Q,  //Recovers 25% of data
    H   //Recovers 30% of data
};

class QREncode{
public:
    QREncode(const string& imageFilePath):_imageFilePath(imageFilePath){}
    ~QREncode(){}
    string encoding(const string& encodeData);
    string decoding(const string& outputPath);
    Matrix<RGB> getFileRGB();
private:
    string _imageFilePath;
    int sideLength{17};
    ErrorCorrectionLevel errorCorLevel{ErrorCorrectionLevel::L};
};

NAME_SPACE_END()

#endif //!_QR_ENCODE_