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

void print(double** input){
	cout<<endl;
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			cout<<input[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

int main(){
	string str="../img/Image/1.jpg";
	JPEGData data;
	data.readJPEG(str.c_str());

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
