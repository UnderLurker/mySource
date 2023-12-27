#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include "jpeg.h"
#include "Image.h"
using namespace std;
using namespace myUtil;

string dataSet="@W#$OEXC[(/?=^~_.` ";

int main()
{
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
    return 0;
}
