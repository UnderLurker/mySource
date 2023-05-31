#include "QrEncode.h"
#include "Image.h"
#include "Matrix.h"
#include "Util.h"
#include <algorithm>
#include <bitset>
#include <stack>

NAME_SPACE_START(myUtil)

Matrix<RGB> QREncode::encoding(const string &encodeData){
    this->encodeData=encodeData;
    if(!init()) return Matrix<RGB>();
    imgData=new Matrix<RGB>(getSideLen(),getSideLen(),RGB_WHITE);

    // char encoding
    string code=bitset<MODE_INDICATOR_BIT_LENGTH>(ModeIndicator[(int)type]).to_string();
    code.append(bitset<16>(encodeData.size()).to_string().substr(16-charCount));
    if(type==DataType::NumMode) code.append(NumModeEncoding());
    else if(type==DataType::AlpNumMode) code.append(AlpNumModeEncoding());
    else if(type==DataType::ByteMode) code.append(ByteModeEncoding());
    else if(type==DataType::KanjiMode) code.append(KanjiModeEncoding());



    return imgData==nullptr?Matrix<RGB>():*imgData;
}
//默认找最小的版本
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
    if (len > 7089 && type == DataType::NumMode ||
        len > 4296 && type == DataType::AlpNumMode ||
        len > 2953 && type == DataType::ByteMode ||
        len > 1817 && type == DataType::KanjiMode)
        return false;
    for(int i=0;i<VERSION_COUNT;i++){
        for(int j=3;j>=0;j--){
            if(CharCapTable[i][j][(int)type]>len){
                version=i+1;
                level=(ErrorCorrectionLevel)j;
                if(version>=1&&version<=9) 
                    charCount=CharCountIndicator[0][(int)type];
                else if(version>=10&&version<=26)
                    charCount=CharCountIndicator[1][(int)type];
                else if(version>=27&&version<=40)
                    charCount=CharCountIndicator[2][(int)type];
                return true;
            }
        }
    }
    return false;
}

string QREncode::NumModeEncoding(){
    string res="";
    //分为3个一组，最后会出现2个一组或者一个一组的
    //都转为10进制，然后3位的转为10位二进制，两位转为7位，一位转为4位
    for (int i = 0; i < encodeData.size(); i += 3) {
        int t=0, bitCount=4;
        if (i < encodeData.size()) t += encodeData[i] - '0';
        if (i + 1 < encodeData.size()){
            t = t * 10 + encodeData[i + 1] - '0';
            bitCount=7;
        }
        if (i + 2 < encodeData.size()){
            t = t * 10 + encodeData[i + 2] - '0';
            bitCount=10;
        }
        if(bitCount==4) res.append(bitset<4>(t).to_string());
        else if(bitCount==7) res.append(bitset<7>(t).to_string());
        else if(bitCount==10) res.append(bitset<10>(t).to_string());
    }
    return res;
}
string QREncode::AlpNumModeEncoding(){
    string res="";
    //两位转11位二进制 一位转6位二进制
    for(int i=0;i<encodeData.size();i+=2){
        int t=0, bitCount=6;
        if(i<encodeData.size()) t = AlpValMappingTable[encodeData[i]];
        if(i+1<encodeData.size()){
            t = t*45 + AlpValMappingTable[encodeData[i+1]];
            bitCount=11;
        }
        if(bitCount==6) res.append(bitset<6>(t).to_string());
        else if(bitCount==11) res.append(bitset<11>(t).to_string());
    }
    return res;
}
string QREncode::ByteModeEncoding(){
    // 1.Convert to ISO 8859-1 or UTF-8
    // 2.Split the String into 8-bit Bytes
    // 3.Convert Each Byte into Binary
    // 4.Next: Finish the Data Encoding Step
    string res="";
    for(int i=0;i<encodeData.size();i++){
        res.append(bitset<8>(encodeData[i]).to_string());
    }
    return res;
}
string QREncode::KanjiModeEncoding(){
    string res="";
    return res;
}

NAME_SPACE_END()