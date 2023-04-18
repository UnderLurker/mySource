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
	string str="../img/Image/1.jpg";
	JPEGData data;
	clock_t startTime=clock();
	data.readJPEG(str.c_str());

	BMPData bmp(data.getRGBMatrix(),data.getWidth(),data.getHeight(),true);
	bmp.GrayEncoder();
	bmp.GaussianHandle(false,[](double in){
		return in<THRESHOLD?255.0:0.0;
		// return in;
	});
	bmp.saveBMP("gray.bmp");
	double m1[][3]={{-1,0,1},{-1,0,1},{-1,0,1}},
		   m2[][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
	bmp.EdgeDetect(false,m1,m2,3);
	bmp.saveBMP("edge.bmp");
	cout<<dec<<clock()-startTime<<"ms"<<endl;
    return 0;
}
