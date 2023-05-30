#include "QrEncode.h"
#include "Matrix.h"
#include "Util.h"
#include <algorithm>

NAME_SPACE_START(myUtil)

Matrix<int> QREncode::encoding(const string &encodeData){
    this->encodeData=encodeData;
    if(!init()) return Matrix<int>();
    
    return Matrix<int>();
}

bool QREncode::init(){
    int NumMode=0,AlpNumMode=0,ByteMode=0;
    for_each(encodeData.begin(), encodeData.end(), [&](char ch){
        if(ch<='9'&&ch>='0') NumMode++;
        else if((ch<='z'&&ch>='a')||(ch<='Z'&&ch>='A')) ByteMode++;
    });
    if(NumMode!=0&&ByteMode!=0) type=DataType::AlpNumMode;
    else if(NumMode!=0) type=DataType::NumMode;
    else if(ByteMode!=0) type=DataType::ByteMode;
    else type=DataType::KanjiMode;
    int len=encodeData.size();
    if(len>7089) return false;
    for(int i=0;i<VERSION_COUNT;i++){
        for(int j=3;j>=4;j--){
            if(CharCapTable[i][j][type]>len){
                version=i+1;
                level=(ErrorCorrectionLevel)j;
                return true;
            }
        }
    }
    return false;
}

NAME_SPACE_END()