# mySource

自己写的一些小工具, 里面有一个简易的服务器，写代码的方式是按照spring boot的方式来的，就是需要继承控制器的基类，至于spring boot中的注释功能是通过宏定义的方式实现的

1. <strong>jpeg解析器编码器(只支持精度为8的)</strong>，JPEGData可以解析YUV为111，411，422等类型图片，但是性能有待提升，解析2.45MB图片耗时10941ms（debug下）使用方法如下：<br/>
```cpp
string str="../img/Image/2.jpg";
JPEGData data;
data.read(str.c_str());//此方法使用之后解析完毕
data.getRGBMatrix();//此公有方法可以获取图像的RGB信息, 返回信息为RGB的二维矩阵(封装一次后的), 此后的BMP也是使用此图像信息来编码
```
JPEGData也可以编码JPEG图片, 支持自定义YUV的比例，注意，Y最大，UV不可为0，使用方法如下<br>
```cpp
string str="../img/Image/2.jpg";
JPEGData data;
data.read(str.c_str());
int input[3][2]={//此数组为YUV的水平和垂直采样因子
    {2,2},  //Y 顺序为水平采样因子，垂直采样因子
    {1,1},  //U
    {1,1}   //V
};
data.write("../img/Image/9.jpg",input, 99);//第三个参数为压缩比例，数值越大压缩越多，第一个参数为输出文件
```

2. <strong>bmp编码器</strong>，提供高斯模糊处理算法（可以自定义二维高斯分布的矩阵大小），和灰度处理<br>使用方法如下<br>
```cpp
//第一个参数为JPEG解析出来的RGB信息，第二个是输入图片宽度，第三个是输入图片高度，第四个为是否输出灰度图
BMPData bmp(data.getRGBMatrix(),data.getWidth(),data.getHeight(),true);
bmp.GrayEncoder();//灰度编码，有两个参数，第二个为灰度算法，第一个为对灰度算法结果的处理
bmp.GaussianHandle(false,[](double in){//高斯编码 第三个参数为输出哪个通道的值 第一个参数为是否对RGB图像进行高斯模糊
    return in>THRESHOLD?255.0:0.0;
    // return in;
},0);
bmp.write("gray.bmp");
```
添加了边缘检测算法，支持<strong>Prewitt Roberts Sobel算子</strong><br>
使用方法如下<br>
```cpp
// prewitt
bmp.EdgeDetect(Prewitt1,Prewitt2,3,[](double i1,double i2){//前两个参数为两个算子
    return (uint8_t)(max(abs(i1),abs(i2)));//是对算子取值的算法
});
bmp.write("prewitt.bmp");
```
2. 图片类都有一个基类Image，所有图片类通用rgb二维矩阵来转换图片类型，如下图</br>具体转换请看[测试程序](./test/Image_test.cpp)
```c++
string str = "../img/Image/4.jpg";
JPEGData data;
data.read(str.c_str());

BMPData bmp(data.getRGBMatrix(), data.getWidth(), data.getHeight(), true);
bmp.GrayEncoder();
bmp.write("jpeg2bmpGray.bmp");
```