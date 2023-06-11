#pragma once
#include <cstdint>
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
// #define _WRITE_DEBUG_

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

static uint8_t YQualityTable[64] = {
    16, 11, 10, 16, 24,  40,  51,  61,  
	12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,  
	14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,  
	24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 
	72, 92, 95, 98, 112, 100, 103, 99};

static uint8_t CQualityTable[64]={
	17, 18, 24, 47, 99, 99, 99, 99,
	18, 21, 26, 66, 99, 99, 99, 99,
	24, 26, 56, 99, 99, 99, 99, 99,
	47, 66, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99
};

static const uint8_t bits_dc_luminance[16] = { /* 0-base */ 
	0, 1, 5, 1, 1, 1, 1, 1, 
	1, 0, 0, 0, 0, 0, 0, 0 
};
static const uint8_t val_dc_luminance[] = { 
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};

static const uint8_t bits_dc_chrominance[16] = { /* 0-base */ 
	0, 3, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 0 
};
static const uint8_t val_dc_chrominance[] = { 
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};

static const uint8_t bits_ac_luminance[16] = { /* 0-base */ 
	0, 2, 1, 3, 3, 2, 4, 3, 
	5, 5, 4, 4, 0, 0, 1, 0x7d 
};
static const uint8_t val_ac_luminance[] = { 
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
};

static const uint8_t bits_ac_chrominance[16] = { /* 0-base */ 
	0, 2, 1, 2, 4, 4, 3, 4, 
	7, 5, 4, 4, 0, 1, 2, 0x77 
};
static const uint8_t val_ac_chrominance[] = { 
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
};

static const int Zig[64]={
	0, 1, 8, 16, 9, 2, 3, 10,
	17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t APP[18]={
	0xff,0xe0,0x00,0x10,0x4a,
	0x46,0x49,0x46,0x00,0x01,
	0x01,0x00,0x00,0x01,0x00,
	0x01,0x00,0x00
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
    COM     = 0xFE,     //注释
	FLAG	= 0xFF		//开始
};

//将一维数组变为二维数组
double** UnZigZag(int* originArray);

struct RGB{
	uint8_t red{0};
	uint8_t green{0};
	uint8_t blue{0};
	uint8_t alpha{0};
	RGB(){}
	RGB(uint8_t r,uint8_t g,uint8_t b,uint8_t a=0):red(r),green(g),blue(b),alpha(a){}
};

static const RGB RGB_BLACK = RGB(0,0,0);
static const RGB RGB_WHITE = RGB(255,255,255);
static const RGB RGB_RED = RGB(255,0,0);
static const RGB RGB_GREEN = RGB(0,255,0);
static const RGB RGB_BLUE = RGB(0,0,255);

//放大图像的矩阵，比例大于1且为整数
template<typename T>
Matrix<T> AmplifyMatrix(T **rgb, int width, int height, int scale = 1){
    int resWidth = width * scale, resHeight = height * scale;
    Matrix<T> res(resHeight, resWidth);
    for (int i = 0; i < resHeight; i++) {
        for (int j = 0; j < resWidth; j++) {
            res.setValue(i, j, rgb[i / scale][j / scale]);
        }
    }
    return res;
}
template<typename T>
Matrix<RGB> AmplifyMatrix(Matrix<T>& rgb, int scale = 1){
    int resWidth = rgb.col * scale, resHeight = rgb.row * scale;
    Matrix<T> res(resHeight, resWidth);
    for (int i = 0; i < resHeight; i++) {
        for (int j = 0; j < resWidth; j++) {
            res.setValue(i, j, rgb.getValue(i/scale, j/scale));
        }
    }
    return res;
}


//SOS
class JPEGScan{
public:
	//componentId,<DC,AC>
	map<uint8_t,pair<uint8_t,uint8_t>> componentHuffmanMap;
	bool Init(fstream& file,uint16_t len);
	string to_string();
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
	static void write(fstream& file);
};
//编码Huffman表
struct JPEGHuffmanEncode{
	//<weight,<code,bit>
	map<uint8_t,pair<uint16_t,uint8_t>> table;
};
//DQT
//quality table
class JPEGQuality{
public:
	uint8_t precision;
	uint8_t id;
	vector<uint16_t> table;
	bool Init(fstream& file,uint16_t len);
	static void write(fstream& file);
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
	int max_h_samp_factor;		//行MCU
	int max_v_samp_factor;		//列MCU
	int width;					//图片宽度
	int height;					//图片高度
	int mcu_rows;				//有多少行mcu
	int mcu_cols;				//有多少列mcu
	int precision;				//精度
	bool isYUV411=false;
	bool isYUV422=false;
	bool isYUV111=false;
	uint8_t curDRI=0;					//当前重置直流分量标识,这里只取个位方便计算
	uint16_t resetInterval=0;			//单位是MCU
	int preDCValue[3]={0};  		//用于直流差分矫正
	vector<JPEGQuality> quality;		//量化表
	vector<JPEGHuffmanCode> dc_huffman;	//解码huffman码表
	vector<JPEGHuffmanCode> ac_huffman;
	vector<JPEGHuffmanEncode> en_dc_huffman;//编码Huffman表
	vector<JPEGHuffmanEncode> en_ac_huffman;
	vector<JPEGComponent> component;	//component每个颜色分量
	JPEGScan scan;
	vector<double**> ycbcr;
	vector<RGB**> rgb;					//解析出来的rgb信息
	double** DCTAndIDCTArray;
	streampos pos;						//文件的总大小
	bool EOI{false};					//是否
	int write_samp_factor[3][2]={0};//生成图片的采样因子
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
	//解析图片
	bool readJPEG(const char* filePath);
	//生成图片,采样因子h v
	bool writeJPEG(const char* filePath, int samp_factor[3][2], int quality_scale);

	int getWidth() const {return width;}
	int getHeight() const {return height;}
	vector<RGB**> getRGB() const{return rgb;}//获取MCU形式的RGB
	Matrix<RGB> getRGBMatrix();//获取通用的RGB数据
	int getMaxHSampFactor() const {return max_h_samp_factor;}
	int getMaxVSampFactor() const {return max_v_samp_factor;}

	double** createDCTAndIDCTArray(int row);
	void DCT(double** originMatrix);
	void IDCT(double** originMatrix);
protected:
	bool readSOF(fstream& file,uint16_t len);
	bool readData(fstream& file);
	bool huffmanDecode(fstream& file);
	void deQuality(double** originMatrix,int qualityID);
	void Quality(double* originMatrix,int qualityID);
	void PAndNCorrect(double** originMatrix);//隔行正负纠正

	RGB** YCbCrToRGB(const int* YUV);
	void RGBToYCbCr(Matrix<RGB> _rgb,fstream& file);

	//标记位检查 是否结束,是否重置直流矫正数值，返回要添加的数值
	string FlagCheck(fstream& file,int byteInfo);
	uint16_t ReadByte(fstream& file,int len);
	uint16_t findHuffmanCodeByBit(fstream& file,int& length,int& pos,string& deque,int curValue,int& curValLen);
	void createDCEnHuffman();
	void createACEnHuffman();
private:
	void initQualityTable(int quality_scale);
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

static double Prewitt1[3][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
static double Prewitt2[3][3]={{-1,0,1},{-1,0,1},{-1,0,1}};

static double Roberts1[3][3]={{1,0},{0,-1}};
static double Roberts2[3][3]={{0,1},{-1,0}};

static double Sobel1[3][3]={{-1,-2,1},{0,0,0},{1,2,1}};
static double Sobel2[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};

//此类只编码倒立图像，即height>0
class BMPData{
	int dataSize{0};					//数据总大小
	int rowSize{0};						//一行有多少个字节（是4的整数倍）
	int width{0};
	int height{0};
	int offbits{54};					//文件头到位图数据的偏移量
	int flag{0};						//图像使用哪一通道，0三通道，1 blue，2 green，3 red
	Matrix<RGB> buf;					//rgb颜色信息
	Matrix<uint8_t> *grayBuf{nullptr};	//灰度图信息
	Matrix<RGB> *rgb{nullptr};			//图像的rgb信息
	bool gray{false};					//false 灰度有调色板 true 彩色没有调色板
	bool forward{true};					//true 为height>0数据表示从图像左下到右上
	uint8_t* bitmap{nullptr};			//最终的数据
	bool isEncode{true};				//是否为编码
	struct Palette{
		uint8_t rgbBlue;
		uint8_t rgbGreen;
		uint8_t rgbRed;
		uint8_t rgbAlpha;
	};
	struct BMPHeader{
		uint32_t bfHeadSize;
		uint16_t bfReserved1;
		uint16_t bfReserved2;
		uint32_t bfOffBits;
		uint32_t biHeadSize;
		uint32_t biWidth;
		uint32_t biHeight;
		uint16_t biPlanes;
		uint16_t biBitCount;
		uint32_t biCompression;
		uint32_t biSizeImage;
		uint32_t biXPelsPerMeter;
		uint32_t biYPelsPerMeter;
		uint32_t biClrUsed;
		uint32_t biClrImportant;
	};
	vector<Palette> palettes;			//调色板信息
	BMPHeader bmpHeader;
public:
	BMPData(){
		isEncode=false;
	}
	//重新编码 gray false灰度 true24位彩色这个gray是决定最后生成的图像是否为灰度图像
	BMPData(const Matrix<RGB>& _buf,int _width,int _height,bool _gray=false)
		:buf(_buf),width(_width),height(_height),gray(_gray){
			Init();
			isEncode=true;
			if(_gray) grayBuf=new Matrix<uint8_t>(height,width,0);
		}
	~BMPData(){
		if(isEncode){
			if (bitmap) delete[] bitmap;
			if (rgb) delete rgb;
			if (grayBuf) delete grayBuf;
		}
	}
	void readBMP(const string& filePath);
	//灰度化,对还没有转成位图数据的
	void GrayEncoder(double (*convert)(double)=[](double in){return in;},
						double (*GrayAlgorithm)(RGB)=[](RGB in){return (in.blue+in.red+in.green)/3.0;});
	//高斯模糊处理，isRGB表示当前是对彩色图像还是对灰度图像进行处理
	void GaussianHandle(bool isRGB,
						double (*convert)(double)=[](double in){return in;},
						int flag=0);
	//边缘检测,只对灰度图像
	void EdgeDetect(double matrix1[3][3],double matrix2[3][3],int row,uint8_t (*algorithm)(double,double)); 
	void saveBMP(const string& filename);

	Matrix<uint8_t> getGray(){return *grayBuf;}
protected:
	void Init();
	// RGB getRGB(int mcu_height,int mcu_width,int row,int col);
	void SetBitmapInfo(int dataSize,int width,int height);

	void analysisHead(uint8_t *head);
};

NAME_SPACE_END()

#endif //!_IMAGE_
