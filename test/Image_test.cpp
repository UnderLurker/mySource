#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdlib.h>
#include "Image.h"
using namespace std;
using namespace myUtil;

void print(double** input,int n){
	cout<<endl;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			cout<<input[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

int main(){
	// double** input=getTwoDimGaussianDistrbute(1,4);
	// print(input,9);
	// FREE_LP_2(input,9)

	string str="../img/Image/2.jpg";
	JPEGData data;
	clock_t startTime=clock();
	data.readJPEG(str.c_str());

	BMPData bmp(data.getRGB(),data.getWidth(),data.getHeight(),false);
	bmp.EncoderByJPEG(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor());
	// bmp.GaussianEncoderByJPEG(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor());
	// bmp.GrayEncoderByJPEG(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor(),
	// 		[](double in){
	// 			// return in>THRESHOLD?255.0:0.0;
	// 			return in;
	// 		});
	// bmp.GaussianByGray();
	bmp.saveBMP("abc.bmp");
	cout<<dec<<clock()-startTime<<"ms"<<endl;

	// DCT正反变换测试
	// JPEGData data;
	// double** arr=new double*[8];
	// for(int i=0;i<8;i++){
	// 	arr[i]=new double[8];
	// 	for(int j=0;j<8;j++){
	// 		arr[i][j]=(int)(rand()%100);
	// 	}
	// }
	// print(arr);
	// data.DCT(arr);
	// print(arr);
	// data.IDCT(arr);
	// print(arr);
	// FREE_LP_2(arr,8)

    return 0;
}
