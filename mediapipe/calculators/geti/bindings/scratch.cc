#include "bindings.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


int main() {
    std::cout << "scratch" << std::endl;

    //std::string image_path = "C:\\data\\cattle.jpg";

    //auto image = cv::imread(image_path);
    //std::cout << image.size() << std::endl;

    //// yuck...
    //std::vector<uchar> buf;
    //cv::imencode(".jpg", image, buf);
    //std::vector<char> image_data(buf.begin(), buf.end());

    std::string result = GraphRunner_Run("detection", "C:/data/detection_ssd.xml", "C:/data/cattle.jpg");

    //runner.OpenGraph("detection", "C:/data/detection_ssd.xml");
    //runner.Queue(image_data);
    //auto result = runner.Get();


    return 0;
}
