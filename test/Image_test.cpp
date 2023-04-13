#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdlib.h>
#include "Image.h"
#include "BmpEncoder.h"
using namespace std;
using namespace myUtil;

// void print(double** input){
// 	cout<<endl;
// 	for(int i=0;i<8;i++){
// 		for(int j=0;j<8;j++){
// 			cout<<input[i][j]<<" ";
// 		}
// 		cout<<endl;
// 	}
// 	cout<<endl;
// }

int main(int argc,char* argv[]){
	// if(argc<3){
	// 	cout<<"for example: ./main.exe 1.jpg 2.bmp"<<endl;
	// 	return 0;
	// }
	// string input(argv[0]),output(argv[1]);
	string input="../img/Image/1.jpg",output="out.bmp";
	JPEGData data;
	clock_t startTime=clock();
	data.readJPEG(input.c_str());
	int size;
	unsigned char *bitmap = Encoder(data.getRGB(), data.getHeight(), data.getWidth(),
								8*data.getMaxHSampFactor(),
								8*data.getMaxVSampFactor(), size);
	Write(output.c_str(), bitmap, size);
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
