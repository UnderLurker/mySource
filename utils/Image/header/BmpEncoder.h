
#pragma once
#include <stdio.h>
#include <iostream>
#include "Image.h"
#include "ImageConfig.h"
#include "Util.h"
using namespace myUtil;


/* Bitmap Header, 54 Bytes  */
static 
unsigned char BmpHeader[54] = 
{
	0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x60, 0xCD, 0x04, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void SetBitmapInfo(unsigned int size, int height, int width)
{
	for (int i = 0; i < 4; i++)
	{
		// size of image ( header + data )
		BmpHeader[2 + i] = size & 0xff;
		size >>= 8;

		// width of image
		BmpHeader[18 + i] = width & 0xff;
		width >>= 8;

		// height of image
		BmpHeader[22  + i] = height & 0xff;
		height >>= 8;
	}
}

RGB getRGB(const vector<RGB**>& buf,int width,int mcu_height,int mcu_width,int row,int col){
	int offsetHeight = (int)floor(row*1.0/mcu_height)*(int)ceil(width*1.0/mcu_width);
	int pos=col/mcu_width+offsetHeight;
	if(pos>=buf.size()) pos=buf.size()-1;
	return buf[pos][row%mcu_height][col%mcu_height];
}

#define THRESHOLD 128

double convert(double input){
	// return round(input)>THRESHOLD?255:0;
	return round(input);
}

/* BGR format */
unsigned char *Encoder(const vector<RGB**>& buf, int height, int width, int mcu_height, int mcu_width, int &size,int flag=0)
{
	uint8_t *bitmap = nullptr;
	int rowSize = (24 * width + 31) / 32 * 4;
	
	// compute the size of total bytes of image
	size = rowSize * height + 54; // data size + header size
	bitmap = new uint8_t [ size ];
	
	// set the header info
	SetBitmapInfo(size, height, width);

	// fill the header area
	for (int i = 0; i < 54; i++)
	{
		bitmap[i] = BmpHeader[i];
	}

	// fill the data area
	for (int i = 0; i < height; i++)
	{
		// compute the offset of destination bitmap and source image
		int idx = height - 1 - i;
		int offsetDst = idx * rowSize + 54; // 54 means the header length
		int offsetHeight = (int)floor(i*1.0/mcu_height)*(int)ceil(width*1.0/mcu_width);
		// fill data
		for (int j = 0; j < width * 3; j++)
		{
			int pos=(j/3)/mcu_width+offsetHeight;
			if(pos>=buf.size()) pos=buf.size()-1;
			RGB temp=buf[pos][i%mcu_height][(j/3)%mcu_height];
			if(j%3==0&&(flag==0||flag==1)) bitmap[offsetDst + j] = temp.blue;
			else if(j%3==1&&(flag==0||flag==2)) bitmap[offsetDst + j] = temp.green;
			else if(j%3==2&&(flag==0||flag==3)) bitmap[offsetDst + j] = temp.red;
			// cout<<dec<<pos<<" ";
		}
		// fill 0x0, this part can be ignored
		for (int j = width * 3; j < rowSize; j++)
		{
			bitmap[offsetDst +j] = 0x0;
		}
	}
	return bitmap;
}

/* BGR format */
unsigned char *gaussianEncoder(const vector<RGB**>& buf, int height, int width, int mcu_height, int mcu_width, int &size,int flag=0)
{
	uint8_t *bitmap = nullptr;
	int rowSize = (24 * width + 31) / 32 * 4;
	
	// compute the size of total bytes of image
	size = rowSize * height + 54; // data size + header size
	bitmap = new uint8_t [ size ];
	
	// set the header info
	SetBitmapInfo(size, height, width);

	// fill the header area
	for (int i = 0; i < 54; i++)
	{
		bitmap[i] = BmpHeader[i];
	}
	
	//高斯变换矩阵
	int gaussianTemplateRadius=2;
	double** gaussian=getTwoDimGaussianDistrbute(1,gaussianTemplateRadius);

	// fill the data area
	for (int i = 0; i < height; i++)
	{
		// compute the offset of destination bitmap and source image
		int idx = height - 1 - i;
		int offsetDst = idx * rowSize + 54; // 54 means the header length
		// fill data
		for (int j = 0; j < width; j++)
		{
			double sum[3]={0};
			int count=0;
			for(int x=i-gaussianTemplateRadius;x<=i+gaussianTemplateRadius;x++){
				for(int y=j-gaussianTemplateRadius;y<=j+gaussianTemplateRadius;y++){
					if(x<0||y<0||y>=width||x>=height) continue;
					RGB temp=getRGB(buf,width,mcu_height,mcu_width,x,y);
					int row=x+gaussianTemplateRadius-i,
						col=y+gaussianTemplateRadius-j;
					sum[0]+=(temp.red*gaussian[row][col]);
					sum[1]+=(temp.green*gaussian[row][col]);
					sum[2]+=(temp.blue*gaussian[row][col]);
					// cout<<"("<<x<<","<<y<<") ";
					count++;
				}
			}
			for(int k=0;k<3;k++){
				if(k==0&&(flag==0||flag==1)) bitmap[offsetDst + j*3 + k] = convert(sum[0]);
				else if(k==1&&(flag==0||flag==2)) bitmap[offsetDst + j*3 + k] = convert(sum[1]);
				else if(k==2&&(flag==0||flag==3)) bitmap[offsetDst + j*3 + k] = convert(sum[2]);
			}
			// cout<<"("<<i<<","<<j<<") ";
		}
		// fill 0x0, this part can be ignored
		for (int j = width * 3; j < rowSize; j++)
		{
			bitmap[offsetDst +j] = 0x0;
		}
	}
	FREE_LP_2(gaussian,2*gaussianTemplateRadius+1)
	return bitmap;
}

/* Save to file */
void Write(const char *fileName, uint8_t *buf, int &size)
{
	FILE *fp = fopen(fileName, "wb+");
	fwrite(buf, 1, size, fp);
	fclose(fp);
}

