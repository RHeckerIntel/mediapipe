#include <iostream>

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"

#include <blend2d.h>

int main() {
    std::cout << "hello world" << std::endl;
    cv::Mat cv_input = cv::imread("/data/cattle.jpg");
    cv::cvtColor(cv_input, cv_input, cv::COLOR_BGR2RGB); // this happens inside the ModelInferRequest
    cv::Mat cv_image;
    cv::cvtColor(cv_input, cv_image, cv::COLOR_RGB2BGRA);
    BLImage img;
    img.createFromData(cv_image.cols, cv_image.rows, BLFormat::BL_FORMAT_XRGB32, cv_image.data, cv_image.step);

    img.writeToFile("/data/output.png");


    return 0;
}
