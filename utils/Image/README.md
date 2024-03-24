# Image

本工具用来解码和编码图片信息，目前支持bmp和jpeg，不同格式通过getRGBMatrix来相互传递rgb数据<br>

示例如下:
```c++
string str = "../../../img/Image/4.jpg";
JPEGData data;
data.read(str.c_str());

BMPData bmp(data.getRGBMatrix(), data.getWidth(), data.getHeight(), true);
bmp.GrayEncoder();
bmp.write("jpeg2bmpGray.bmp");
```
bmp支持灰度化，边缘检测(支持Prewitt算子，Roberts算子，Sobel算子)，高斯模糊等，为BMPData的成员方法
```c++
string str = "../../../img/Image/5.bmp";
BMPData bmpData;
bmpData.read(str.c_str());

BMPData newData(bmpData.getRGBMatrix(), bmpData.getWidth(), bmpData.getHeight(), true);
newData.GrayEncoder();
newData.write("bmp2bmpGray.bmp");
```

# QRCode

详情见[测试程序](./test/QrCode_test.cpp)