#pragma once
#include <fstream>
#include <stdint.h>
#include <utility>
#ifndef _IMAGE_
#define _IMAGE_

#include "Util.h"
#include <string>
#include <vector>
#include <math.h>
#include <iostream>
using namespace std;

NAME_SPACE_START(myUtil)

#define HUFFMAN_DECODE_DEQUE_CACHE 64//单位：位
// #define _DEBUG_
// #define _DEBUGOUT_

//用于做dct逆变换
#define _A_ cos(M_PI / 4) / 2
#define _B_ cos(M_PI / 16) / 2
#define _C_ cos(M_PI / 8)
#define _D_ cos(M_PI*3 / 16) / 2
#define _E_ cos(M_PI*5 / 16) / 2
#define _F_ cos(M_PI*3 / 8) / 2
#define _G_ cos(M_PI*7 / 16) / 2
const double IDctArray[8][8] = {
	{_A_, _A_, _A_, _A_, _A_, _A_, _A_, _A_},
	{_B_, _D_, _E_, _G_,-_G_,-_E_,-_D_,-_B_},
	{_C_, _F_,-_F_,-_C_,-_C_,-_F_, _F_, _C_},
	{_D_,-_G_,-_B_,-_E_, _E_, _B_, _G_,-_D_},
	{_A_,-_A_,-_A_, _A_, _A_,-_A_,-_A_, _A_},
	{_E_,-_B_, _G_, _D_,-_D_,-_G_, _B_,-_E_},
	{_F_,-_C_, _C_,-_F_,-_F_, _C_,-_C_, _F_},
	{_G_,-_E_, _D_,-_B_, _B_,-_D_, _E_,-_G_}
};

//段类型
enum JPEGPType{
    SOF0    = 0xC0,     //帧开始
    SOF1    = 0xC1,     //帧开始
    SOF2    = 0xC2,     //帧开始
    DHT     = 0xC4,     //哈夫曼表
    SOI     = 0xD8,     //文件头
    EOI     = 0xD9,     //文件尾
    SOS     = 0xDA,     //扫描行开始
    DQT     = 0xDB,     //定义量化表
    DRI     = 0xDD,     //定义重新开始间隔
    APP0    = 0xE0,     //定义交换格式和图像识别信息
    APP1    = 0xE1,     //定义交换格式和图像识别信息
    APP2    = 0xE2,     //定义交换格式和图像识别信息
    COM     = 0xFE      //注释
};

uint16_t ReadByte(fstream& file,int len);
uint16_t findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen,bool flag);
//将一维数组变为二维数组
int** UnZigZag(int* originArray);
//真正的反dct变换
void IDCT(int** originMatrix);

//SOS
class JPEGScan{
public:
	//componentId,<DC,AC>
	map<uint8_t,pair<uint8_t,uint8_t>> componentHuffmanMap;
	bool Init(fstream& file,uint16_t len);
};

//APP
class JPEGInfo{
public:
	uint16_t version;

};
//DHT
class JPEGHuffmanCode{
public:
	using iterator = map<uint16_t,pair<uint8_t,uint8_t>>::iterator;
	//<code,<bit,weight>
	map<uint16_t,pair<uint8_t,uint8_t>> table;
	//init huffman table
	bool Init(fstream& file,uint16_t len);
	//find-true not find-false
	bool findKey(const uint16_t& code,const uint8_t& bit,iterator& it);
};
//DQT
//quality table
class JPEGQuality{
public:
	uint8_t precision;
	uint8_t id;
	vector<uint16_t> table;
	bool Init(fstream& file,uint16_t len);
};

//SOF segment
class JPEGComponent{
public:
	uint8_t colorId;
	uint8_t h_samp_factor;
	uint8_t v_samp_factor;
	uint8_t qualityId;
	bool Init(fstream& file,uint16_t len);
};

class JPEGData{
	int max_h_samp_factor;//行MCU
	int max_v_samp_factor;//列MCU
	int width;
	int height;
	int precision;
	bool isYUV411=false;
	bool isYUV422=false;
	bool isYUV111=false;
	uint16_t resetInterval=0;//单位是MCU
	//量化表
	vector<JPEGQuality> quality;
	//huffman码表
	vector<JPEGHuffmanCode> dc_huffman;
	vector<JPEGHuffmanCode> ac_huffman;
	//component每个颜色分量
	vector<JPEGComponent> component;
	JPEGScan scan;
	//vector<int**> deHuffman;
	vector<int**> rgb;
public:
	JPEGData():
			max_h_samp_factor(0),
			max_v_samp_factor(0),
			width(0),
			height(0),
			precision(0){}
	bool readJPEG(const char* filePath);
protected:
	bool readSOF(fstream& file,uint16_t len);
	bool readData(fstream& file);
	bool huffmanDecode(fstream& file);
	bool deQuantity();
	bool deZSort();

};

NAME_SPACE_END()

#endif //!_IMAGE_
