#pragma once
#include <cstdint>
#include <fstream>
#include <cstdint>
#include <utility>
#ifndef _IMAGE_
#define _IMAGE_

#include "Util.h"
#include "Matrix.h"
#include "ImageBase.h"
#include "ImageUtil.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

NAME_SPACE_START(myUtil)


struct Palette{
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbAlpha;
};

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
	BMPHeader bmpHeader{};
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
	~BMPData() {
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
