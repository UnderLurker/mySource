#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat img;
    img = cv::imread("../img/Image/1.jpg"); //这里放你图片的绝对路径，注意是双斜杠

    cv::imshow("show", img);
    cv::waitKey(0);

    return 0;
}
