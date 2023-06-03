#include "QrEncode.h"
#include "Image.h"
#include "Matrix.h"
#include "Util.h"
#include <algorithm>
#include <bitset>
#include <stack>
#include <string>

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
    fillEncodeData(code);


    return imgData==nullptr?Matrix<RGB>():*imgData;
}

Matrix<RGB> QREncode::encoding(const wstring& encodeWData){
    this->encodeWData=encodeWData;
    if(!init()) return Matrix<RGB>();
    imgData=new Matrix<RGB>(getSideLen(),getSideLen(),RGB_WHITE);

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
    // 1.Only for Double-Byte Shift JIS Characters
    // 2.Convert to Bytes
    // 3.Encode the Bytes with Kanji Mode
    //     For characters whose bytes are in the range 0x8140 to 0x9FFC
    //      for example: 0x89D7 
    //     1. 0x89D7 - 0x8140 = 0x0897
    //     2. (0x08 * 0xC0) + 0x97 = (0x600) + 0x97 = 0x697
    //     3. 0x697 -> binary(13bit)
    //     For characters whose bytes are in the range 0xE040 to 0xEBBF
    //      for example: 0xE4AA 
    //     1. 0xE4AA - 0xC140 = 0x236A
    //     2. (0x23 * 0xC0) + 0x6A = (0x1A40) + 0x6A = 0x1AAA
    //     3. 0x1AAA -> binary(13bit)
    // 4.Put the 13-bit Binary Numbers Together
    // 5.Next: Finish the Data Encoding Step
    string res="";
    for(int i=0;i<encodeWData.size();i++){

    }
    return res;
}

void QREncode::fillEncodeData(string& code){
    // 1. Determine the Required Number of Bits for this QR Code
    // 2. Add a Terminator of 0s if Necessary
    // 3. Add More 0s to Make the Length a Multiple of 8
    // 4. Add Pad Bytes if the String is Still too Short
    //  11101100 00010001//两字节循环填入
    // code string bit length
    int maxLen=ErrorCurrentTable[(version-1)*4+(int)level][0]*8;
    int diff=maxLen-code.size();
    if(diff<0) return;
    // Add a Terminator
    if(diff>4) code.append("0000");
    else if(diff<=4) code.append(string(diff,'0'));
    // Add More 0s to Make the Length a Multiple of 8
    int eightDiff = 8-code.size()%8;
    code.append(string(eightDiff,'0'));
    // Add Pad Bytes
    if(maxLen-code.size()<8) return;
    for(int i=0;i<(maxLen-code.size())/8;i++){
        if(i%2==0) code.append("11101100");
        else code.append("00010001");
    }
    errorCurrentEncoding(code);
}

void QREncode::errorCurrentEncoding(string& code){
    // Step 1: Break Data Codewords into Blocks if Necessary
    // Step 2: Understand Polynomial Long Division
    // Step 3: Understand The Galois Field
    // Step 4: Understand Galois Field Arithmetic
    // Step 5: Generate Powers of 2 using Byte-Wise Modulo 100011101
    // Step 6: Understand Multiplication with Logs and Antilogs
    // Step 7: Understanding The Generator Polynomial
    // Step 8: Generating Error Correction Codewords
    // Step 9: Divide the Message Polynomial by the Generator Polynomial
    int row=(version-1)*4+(int)level,codePos=0;
    int groupCount = ErrorCurrentTable[row][4]==0?1:2;
    int*** group=new int**[groupCount];
    // first group
    group[0]=new int*[ErrorCurrentTable[row][2]];
    for(int blockOfGroup=0;blockOfGroup<ErrorCurrentTable[row][2];blockOfGroup++){
        group[0][blockOfGroup]=new int[ErrorCurrentTable[row][3]];
        for(int blocks=0;blocks<ErrorCurrentTable[row][3]&&codePos<=code.size();blocks++){
            int t=0;
            for(int k=0;k<8;k++){
                t<<=1;
                t+=code[codePos+k]-'0';
            }
            group[0][blockOfGroup][blocks]=t;
            codePos+=8;
        }
    }
    // second group
    if(groupCount==1) return;
    group[1]=new int*[ErrorCurrentTable[row][4]];
    for(int blockOfGroup=0;blockOfGroup<ErrorCurrentTable[row][4];blockOfGroup++){
        group[1][blockOfGroup]=new int[ErrorCurrentTable[row][3]];
        for(int blocks=0;blocks<ErrorCurrentTable[row][5]&&codePos<=code.size();blocks++){
            int t=0;
            for(int k=0;k<8;k++){
                t<<=1;
                t+=code[codePos+k]-'0';
            }
            group[1][blockOfGroup][blocks]=t;
            codePos+=8;
        }
    }
}

NAME_SPACE_END()