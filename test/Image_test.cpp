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

string dataSet="@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'";

int main(){
	string str="../img/Image/4.jpg";
	JPEGData data;
	clock_t startTime=clock();
	data.readJPEG(str.c_str());
	fstream file("draw.txt",ios::out);

	// int input[3][2]={
	// 	{2,2},
	// 	{1,1},
	// 	{1,1}
	// };
	// data.writeJPEG("../img/Image/9.jpg",input, 99);

	BMPData bmp(data.getRGBMatrix(),data.getWidth(),data.getHeight(),true);
	bmp.GrayEncoder();
	auto temp = bmp.getGray();
	double a=(dataSet.size()*1.0/255);
	for(int i=0;i<temp.row;i+=2){
		for(int j=0;j<temp.col;j++){
			auto t=temp.getValue(i, j);
			int index=(int)(a*(int)t);
			file<<dataSet[index];
		}
		file<<endl;
	}
	// bmp.GaussianHandle(false,[](double in){
	// 	return in>THRESHOLD?255.0:0.0;
	// 	// return in;
	// });
	// bmp.saveBMP("gray.bmp");

	// prewitt
	// bmp.EdgeDetect(Prewitt1,Prewitt2,3,[](double i1,double i2){
	// 	return (uint8_t)(max(abs(i1),abs(i2)));
	// });
	// bmp.saveBMP("prewitt.bmp");

	// // Roberts
	// bmp.EdgeDetect(Roberts1,Roberts2,2,[](double i1,double i2){
	// 	return (uint8_t)(abs(i1)+abs(i2));
	// });
	// bmp.saveBMP("robert.bmp");

	// // Sobel
	// bmp.EdgeDetect(Sobel1,Sobel2,3,[](double i1,double i2){
	// 	return (uint8_t)(abs(i1)+abs(i2));
	// });
	// bmp.saveBMP("abc.bmp");
	cout<<dec<<clock()-startTime<<"ms"<<endl;
    return 0;
}
