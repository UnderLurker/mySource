#include <cstdint>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

constexpr int maxLight = 100;
constexpr int maxContact = 20;
std::string operatorName = "src";
// 显示图片
void showImage(const std::string &name, const cv::Mat &image) {

    cv::namedWindow(name, cv::WindowFlags::WINDOW_AUTOSIZE);
    cv::imshow(name, image);
    //    cv::moveWindow(name, offsetX, offsetY);
}

// 反色
void reflectGray(const cv::Mat &gray, cv::Mat &result) {

    if (gray.channels() != 1)
        return;
    gray.copyTo(result);
    gray.forEach<uchar>([&result](uchar &p, const int position[]) {
        result.at<uchar>(position[0], position[1]) = 0xFF - p;
    });

    // for(int i=0;i<gray.rows;i++) {
    //   uchar* curRow = gray.ptr<uchar>(i);
    //   for(int j=0;j<gray.cols;j++) {
    //     result.at<uchar>(i, j) = 0xFF - *curRow++;
    //   }
    // }
}

// C接口调用封装回调函数中使用的数据
struct UserInfo {
    cv::Mat image;
    int* lightness;
    int* contract;
};

// 滑轨回调
void onLight(int value, void *userdata) {
    UserInfo info = *reinterpret_cast<UserInfo *>(userdata);
    cv::Mat show;
    // 调节亮度
    info.image.convertTo(show, -1, *info.contract / 10.0f, value);
    *info.lightness = value;
    showImage(operatorName, show);
}
void onContract(int value, void *userdata) {
    UserInfo info = *reinterpret_cast<UserInfo *>(userdata);
    cv::Mat show;
    // 调节对比度
    info.image.convertTo(show, -1, value / 10.0f, *info.lightness);
    *info.contract = value;
    showImage(operatorName, show);
}
void onMouse(int event, int x, int y, int flags, void* userdata) {
    UserInfo info = *reinterpret_cast<UserInfo *>(userdata);
    cv::Mat show;
    info.image.copyTo(show);
    cv::circle(show, cv::Point(x, y), 100, cv::Scalar(0, 255, 0), 20);
    showImage(operatorName, show);
}

void photo() {

    std::string imagePath = "../img/Image/3.jpg";
    cv::Mat src = cv::imread(imagePath);
    cv::Mat image;
    if (src.empty()) {
        cout << "Can't load picture." << endl;
        return;
    }

    //    cv::cvtColor(src, image, cv::COLOR_BGR2GRAY);
    //    cv::rectangle(src, cv::Rect(100, 100, 200, 200), cv::Scalar(0, 255, 0), 20);
    //    showImage("origin", src);

    src.copyTo(image);
    vector<cv::Mat> imageV;
    cv::split(image, imageV);
    vector<int> fromTo {0, 2, 1, 1, 2, 0};
    cv::mixChannels(imageV, imageV, fromTo.data(), 3);
    cv::merge(imageV, image);

    cv::resize(src, src, cv::Size(src.cols, src.rows));
    cv::flip(src, src, -1);

    int  angle = 45;
    auto m = cv::getRotationMatrix2D(cv::Point2f {src.cols / 2.0f, src.rows / 2.0f}, angle, 1);
    cv::Size2f newSize;
    newSize.width = src.rows * sin(angle) + src.cols * cos(angle);
    newSize.height = src.cols * sin(angle) + src.rows * cos(angle);
    m.at<double>(0, 2) += (newSize.width - src.cols) / 2;
    m.at<double>(1, 2) += (newSize.height - src.rows) / 2;
    cv::warpAffine(src, src, m,newSize);


    showImage("norm", src);
    //    showImage(operatorName, image);

    //    int lightness = 0;
    //    int contract = 10;
    //    UserInfo info{.image = image, .lightness = &lightness, .contract = &contract};
    //    cv::createTrackbar("lightness", operatorName, &lightness, maxLight, onLight, &info);
    //    cv::createTrackbar("contract", operatorName, &contract, maxContact, onContract, &info);
    //    cv::setMouseCallback(operatorName, onMouse, &info);


    cv::waitKey(0);
    cv::destroyAllWindows();
}

void showHist(const std::string& winName, const cv::Mat& image) {
    vector<cv::Mat> v;
    cv::split(image, v);
    constexpr int width = 256, height = 255;
    cv::Mat bHist, gHist, rHist;
    const int bins[1] = {256};
    float hranges[2] = { 0,255 };
    const float* ranges[1] = { hranges };
    cv::calcHist(&v[0], 1, 0, cv::Mat(), bHist, 1, bins, ranges);
    cv::calcHist(&v[1], 1, 0, cv::Mat(), gHist, 1, bins, ranges);
    cv::calcHist(&v[2], 1, 0, cv::Mat(), rHist, 1, bins, ranges);

    cv::Mat show = cv::Mat::zeros(width, height, CV_8UC3);
    int lineW = width / bins[0];
    cv::normalize(bHist, bHist, 0, height, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(gHist, gHist, 0, height, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(rHist, rHist, 0, height, cv::NORM_MINMAX, -1, cv::Mat());

    for(int i=1;i<bins[0];i++) {
        cv::line(show, cv::Point(lineW * (i - 1), height - cvRound(bHist.at<float>(i - 1))),
                 cv::Point (lineW * i, height - cvRound(bHist.at<float>(i))), cv::Scalar(255, 0, 0));
        cv::line(show, cv::Point(lineW * (i - 1), height - cvRound(gHist.at<float>(i - 1))),
                 cv::Point (lineW * i, height - cvRound(gHist.at<float>(i))), cv::Scalar(0, 255, 0));
        cv::line(show, cv::Point(lineW * (i - 1), height - cvRound(rHist.at<float>(i - 1))),
                 cv::Point (lineW * i, height - cvRound(rHist.at<float>(i))), cv::Scalar(0, 0, 255));
    }

    showImage(winName, show);
}

void playVideo() {
    std::string hist = "HIST";
    cv::VideoCapture cap("D:\\video\\1.mp4");
    cout << cap.get(cv::CAP_PROP_FRAME_WIDTH) << endl;
    cv::Mat video;
    while(true) {
        cap.read(video);
        if (video.empty()) break;
        cv::imshow("video", video);
        showHist(hist, video);
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    cv::destroyAllWindows();
    cap.release();
}

int main() {
    playVideo();
    photo();
    return 0;
}