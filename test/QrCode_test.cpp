#include "Image.h"
#include "QrEncode.h"
#include <cstdint>
#include <fstream>
#include <list>
#include <string>
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
// #include "QrEncode.h"
#include "logger.h"
#include "Matrix.h"
using namespace std;
using namespace myUtil;
void print(Matrix<RGB> res){
    for(int i=0;i<res.row;i++){
        for(int j=0;j<res.col;j++){
            RGB d=res.getValue(i, j);
            cout<<dec<<"("<<(int)d.red<<","
                <<(int)d.green<<","
                <<(int)d.blue<<") ";
        }
        cout << endl;
    }
}
string string_to_hex(const string &str) // transfer string to hex-string
{
    string result ="";
    string tmp;
    stringstream ss;
    int n=str.size();
    for (int i = 0; i < str.size(); i++) {
        ss << hex << int(str[i]) << endl;
        ss >> tmp;
        result += tmp + '\n';
    }
    return result;
}
int main(){
    // QREncode q("../bin/CMakeFiles/Image_static.dir");
    // Logger::Init("../config.ini");
    // Logger::LogStart(L"aaa%saaa",L"main");
    // Logger::Info("info");
    // Logger::Debug("debug");
    // Logger::Warning("warning");
    // Logger::Error("error");
    // Logger::LogEnd("aaa%sbbbb","cccc");
    
    // Matrix<RGB> source(6,6,RGB_WHITE);
    // vector<RGB> rgb;
    // rgb.push_back(RGB_BLACK);
    // vector<vector<int>> input{
    //     {1,0,1,0,1,0},
    //     {0,1,0,1,0,1},
    //     {1,0,1,0,1,0},
    //     {0,1,0,1,0,1},
    //     {1,0,1,0,1,0},
    //     {0,1,0,1,0,1}
    // };
    // Matrix<int> in(6,6,input);
    // source.setValByArray(in, rgb, 0, 0);
    // BMPData bmp(AmplifyMatrix<RGB>(source,100),6*100,6*100,true);
    // bmp.GrayEncoder();
    // bmp.saveBMP("custom.bmp");

    // QREncode code;
    // auto rgb=code.encoding("abcdefghij");
    // BMPData bmp(AmplifyMatrix<RGB>(rgb,20),
    //         code.getSideLen()*20,
    //         code.getSideLen()*20,true);
    // bmp.GrayEncoder();
    // bmp.saveBMP("qrcode.bmp");
    // cout<<string_to_hex("你好");
    // fstream file("./1.txt",ios::in|ios::binary);
    // while(!file.eof()){
    //     cout<<hex<<file.get()<<endl;
    // }
    // cout<<endl;

    // int arr[]={0,1,1,0,0,0,0,0,0,0,0,0,0,0,0};
    // auto res = getErrorCurrentWords(arr, 15, 24);
    // for(int item : res){
    //     cout<<item<<" ";
    // }
    // cout<<endl;

    QREncode qr;
    Matrix<RGB> rgb=qr.encoding("HELLO WORD");
    BMPData bmp(AmplifyMatrix<RGB>(rgb,4),rgb.col*4,rgb.row*4,true);
    bmp.GrayEncoder();
    bmp.saveBMP("qr.bmp");
    return 0;
}
