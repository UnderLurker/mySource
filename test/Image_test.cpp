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
	string str="../img/Image/3.jpg";
	JPEGData data;
	clock_t startTime=clock();
	data.readJPEG(str.c_str());

	BMPData bmp(data.getRGB(),data.getWidth(),data.getHeight(),false);
	// bmp.EncoderByJPEG(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor());
	bmp.GaussianHandle(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor(),true);
	// bmp.GrayEncoderByJPEG(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor(),
	// 		[](double in){
	// 			// return in>THRESHOLD?255.0:0.0;
	// 			return in;
	// 		});
	// bmp.GrayEncoderByBMP(8*data.getMaxVSampFactor(), 8*data.getMaxHSampFactor());
	bmp.saveBMP("abc.bmp");
	cout<<dec<<clock()-startTime<<"ms"<<endl;

    return 0;
}
