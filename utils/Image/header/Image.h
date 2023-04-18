#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <stdint.h>
#include <utility>
#ifndef _IMAGE_
#define _IMAGE_

#include "Util.h"
#include "Matrix.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

NAME_SPACE_START(myUtil)

#define ROW 8
#define COL 8
#define HUFFMAN_DECODE_DEQUE_CACHE 64//单位：位
// #define _DEBUG_
// #define _DEBUGOUT_

// //用于做dct逆变换
// #define _A_ cos(M_PI / 4) / 2
// #define _B_ cos(M_PI / 16) / 2
// #define _C_ cos(M_PI / 8)
// #define _D_ cos(M_PI * 3 / 16) / 2
// #define _E_ cos(M_PI * 5 / 16) / 2
// #define _F_ cos(M_PI * 3 / 8) / 2
// #define _G_ cos(M_PI * 7 / 16) / 2
// static const double IDctArray[8][8] = {
// 	{_A_, _A_, _A_, _A_, _A_, _A_, _A_, _A_},
// 	{_B_, _D_, _E_, _G_, -_G_, -_E_, -_D_, -_B_},
// 	{_C_, _F_, -_F_, -_C_, -_C_, -_F_, _F_, _C_},
// 	{_D_, -_G_, -_B_, -_E_, _E_, _B_, _G_, -_D_},
// 	{_A_, -_A_, -_A_, _A_, _A_, -_A_, -_A_, _A_},
// 	{_E_, -_B_, _G_, _D_, -_D_, -_G_, _B_, -_E_},
// 	{_F_, -_C_, _C_, -_F_, -_F_, _C_, -_C_, _F_},
// 	{_G_, -_E_, _D_, -_B_, _B_, -_D_, _E_, -_G_}};

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

//将一维数组变为二维数组
double** UnZigZag(int* originArray);

struct RGB{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

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
	//1＝Y, 2＝Cb, 3＝Cr, 4＝I, 5＝Q
	uint8_t colorId;
	uint8_t h_samp_factor;
	uint8_t v_samp_factor;
	uint8_t qualityId;
	bool Init(fstream& file,uint16_t len);
};

class JPEGData{
	int max_h_samp_factor;//行MCU
	int max_v_samp_factor;//列MCU
	int width;//图片宽度
	int height;//图片高度
	int mcu_rows;//有多少行mcu
	int mcu_cols;//有多少列mcu
	int precision;//精度
	bool isYUV411=false;
	bool isYUV422=false;
	bool isYUV111=false;
	uint8_t curDRI=0;//当前重置直流分量标识,这里只取个位方便计算
	uint16_t resetInterval=0;//单位是MCU
	int preDCValue[3]={0};  //用于直流差分矫正
	//量化表
	vector<JPEGQuality> quality;
	//huffman码表
	vector<JPEGHuffmanCode> dc_huffman;
	vector<JPEGHuffmanCode> ac_huffman;
	//component每个颜色分量
	vector<JPEGComponent> component;
	JPEGScan scan;
	//vector<int**> deHuffman;
	vector<double**> ycbcr;
	vector<RGB**> rgb;//解析出来的rgb信息
	double** DCTAndIDCTArray;
	streampos pos;//文件的总大小
	bool EOI{false};//是否
public:
	JPEGData():
			max_h_samp_factor(0),
			max_v_samp_factor(0),
			width(0),
			height(0),
			precision(0){
				DCTAndIDCTArray=createDCTAndIDCTArray(ROW);
			}
	~JPEGData(){
		FREE_LP_2(DCTAndIDCTArray,ROW-1)
		// FREE_LP_2(DCTArray,ROW-1)
		// FREE_LP_2(IDCTArray,ROW-1)
		FREE_VECTOR_LP(rgb,ROW)
	}
	bool readJPEG(const char* filePath);

	int getWidth() const {return width;}
	int getHeight() const {return height;}
	//获取MCU形式的RGB
	vector<RGB**> getRGB() const{return rgb;}
	//获取通用的RGB数据
	Matrix<RGB> getRGBMatrix();
	int getMaxHSampFactor() const {return max_h_samp_factor;}
	int getMaxVSampFactor() const {return max_v_samp_factor;}

	double** createDCTAndIDCTArray(int row);
	//double** createIDCTArray(int row);
	void DCT(double** originMatrix);
	void IDCT(double** originMatrix);
protected:
	bool readSOF(fstream& file,uint16_t len);
	bool readData(fstream& file);
	bool huffmanDecode(fstream& file);
	void deQuality(double** originMatrix,int qualityID);
	//隔行正负纠正
	void PAndNCorrect(double** originMatrix);
	RGB** YCbCrToRGB(const int* YUV);
	//标记位检查 是否结束,是否重置直流矫正数值，返回要添加的数值
	string FlagCkeck(fstream& file,int byteInfo);
	uint16_t ReadByte(fstream& file,int len);
	uint16_t findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen);
};

#define GAUSSIAN
#define GAUSSIAN_RADIUS 1
#define GAUSSIAN_TEMPLATE_RADIUS 8
//阈值
#define THRESHOLD 128

struct Palette{
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbAlpha;
};

enum Operator{
	Sobel		=0,
	Roberts		=1,
	Prewitt		=2,
	Laplacian	=3,
	Canny		=4
}; 

/* Bitmap Header, 54 Bytes  */
static unsigned char BmpHeader[54] = 
{
	0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x60, 0xCD, 0x04, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//目前只能允许直接对
class BMPData{
	int dataSize{0};//数据总大小
	int rowSize{0};//一行有多少个字节（是4的整数倍）
	int width{0};
	int height{0};
	int flag{0};//图像使用哪一通道，0三通道，1 blue，2 green，3 red
	Matrix<RGB> buf;//rgb颜色信息
	Matrix<uint8_t> *grayBuf;//灰度图信息
	bool gray{false};//false 灰度有调色板 true 彩色没有调色板
	uint8_t* bitmap{nullptr};//最终的数据
	struct Palette{
		uint8_t rgbBlue;
		uint8_t rgbGreen;
		uint8_t rgbRed;
		uint8_t rgbAlpha;
	};
public:
	//gray false灰度 true24位彩色这个gray是决定最后生成的图像是否为灰度图像
	BMPData(const Matrix<RGB>& _buf,int _width,int _height,bool _gray=false)
		:buf(_buf),width(_width),height(_height),gray(_gray){
			Init();
			grayBuf=new Matrix<uint8_t>(height,width);
		}
	~BMPData(){
		delete [] bitmap;
	}
	//灰度化,对还没有转成位图数据的
	void GrayEncoder(double (*convert)(double)=[](double in){return in;},
						double (*GrayAlgorithm)(RGB)=[](RGB in){return (in.blue+in.red+in.green)/3.0;});
	//高斯模糊处理，isRGB表示当前是对彩色图像还是对灰度图像进行处理
	void GaussianHandle(bool isRGB,
						double (*convert)(double)=[](double in){return in;},
						int flag=0);
	//边缘检测,只对灰度图像
	void EdgeDetectPrewitt(); 
	void saveBMP(const char *fileName);
protected:
	void Init();
	// RGB getRGB(int mcu_height,int mcu_width,int row,int col);
	void SetBitmapInfo(int dataSize,int width,int height);
};

NAME_SPACE_END()

#endif //!_IMAGE_
