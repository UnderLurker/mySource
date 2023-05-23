#include "Image.h"
#include <cstdint>
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
        cout<<endl;
    }
}
int main(){
    // QREncode q("../bin/CMakeFiles/Image_static.dir");
    // Logger a;
    // Logger::Init("F:/Visual-Studio-practice/CC++/mySource/config.ini");
    // Logger::LogStart(L"aaa%saaa",L"main");
    // Logger::Info("info");
    // Logger::Debug("debug");
    // Logger::Warning("warning");
    // Logger::Error("error");
    // Logger::LogEnd("main");
    Matrix<RGB> source(6,6,RGB_WHITE);
    vector<RGB> rgb;
    rgb.push_back(RGB_BLACK);
    rgb.push_back(RGB_BLUE);
    vector<vector<int>> input{
        {1,0,1,0,1,0},
        {0,1,0,1,0,1},
        {1,0,2,0,1,0},
        {0,1,0,1,0,1},
        {1,0,1,0,1,0},
        {0,1,0,1,0,1}
    };
    Matrix<int> in(6,6,input);
    source.setValByArray(in, rgb, 0, 0);
    BMPData bmp(AmplifyMatrix<RGB>(source,100),6*100,6*100,false);
    // bmp.GrayEncoder();
    bmp.saveBMP("custom.bmp");
    return 0;
}