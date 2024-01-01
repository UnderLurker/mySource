#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#define TEST(methodName) void methodName()

using namespace std;
using namespace myUtil;

TEST(jpeg2bmpGray)
{
    string str = "../img/Image/4.jpg";
    JPEGData data;
    clock_t startTime = clock();
    data.read(str.c_str());

    BMPData bmp(data.getRGBMatrix(), data.getWidth(), data.getHeight(), true);
    bmp.GrayEncoder();
    bmp.write("jpeg2bmpGray.bmp");
    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(jpeg2bmp)
{
    string str = "../img/Image/4.jpg";
    JPEGData data;
    clock_t startTime = clock();
    data.read(str.c_str());

    BMPData bmp(data.getRGBMatrix(), data.getWidth(), data.getHeight());
    bmp.write("jpeg2bmp.bmp");
    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(charDraw)
{
    string dataSet="@W#$OEXC[(/?=^~_.` ";
    string str = "../img/Image/4.jpg";
    JPEGData data;
    clock_t startTime = clock();
    data.read(str.c_str());
    fstream file("draw.txt", ios::out);

    BMPData bmp(data.getRGBMatrix(), data.getWidth(), data.getHeight(), true);
    bmp.GrayEncoder();
    auto temp = bmp.getGray();
    double a = (dataSet.size() * 1.0 / 255);
    int32_t length = 1, expect = 2;
    for (int i = 0; i < temp.row; i += length * expect) {
        for (int j = 0; j < temp.col; j += length) {
            auto t = temp.getValue(i, j);
            int index = (int)(a * (int)t);
            file << dataSet[index];
        }
        file << endl;
    }
    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(bmp2bmpGray)
{
    clock_t startTime = clock();
    string str = "../img/Image/5.bmp";
    BMPData bmpData;
    bmpData.read(str.c_str());

    BMPData newData(bmpData.getRGBMatrix(), bmpData.getWidth(), bmpData.getHeight(), true);
    newData.GrayEncoder();
    newData.write("bmp2bmpGray.bmp");
    // JPEGData data;
    // data.setRGBMatrix(bmpData.getRGBMatrix());
    // data.write("bmp2jpeg");

    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(bmp2bmp)
{
    clock_t startTime = clock();
    string str = "../img/Image/5.bmp";
    BMPData bmpData;
    bmpData.read(str.c_str());

    BMPData newData(bmpData.getRGBMatrix(), bmpData.getWidth(), bmpData.getHeight());
    newData.write("bmp2bmp.bmp");

    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(bmp2jpeg)
{
    clock_t startTime = clock();
    string str = "../img/Image/5.bmp";
    BMPData bmpData;
    bmpData.read(str.c_str());

    JPEGData data;
    data.setRGBMatrix(bmpData.getRGBMatrix());
    data.write("bmp2jpeg.jpg");

    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(jpeg2jpeg)
{
    clock_t startTime = clock();
    string str = "bmp2jpeg.jpg";
    JPEGData data;
    data.read(str.c_str());
    auto a = data.write("jpeg2jpeg.jpg");

    cout << dec << clock() - startTime << "ms" << endl;
}

TEST(png)
{
    PNGData pngData;
    pngData.read("../img/Image/6.png");
}

int main()
{
     jpeg2bmp();
     bmp2bmp();
     jpeg2bmpGray();
     bmp2bmpGray();
     bmp2jpeg();
     jpeg2jpeg();
//    png();
    return 0;
}
