#include "bindings.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


#include <thread>

void handle_input(const char* message) {
    std::cout << "got output message" << std::endl;
}

void listen_task(void* instance) {
    GraphRunner_Listen(instance, handle_input);
}

int main() {
    std::cout << "scratch" << std::endl;

    std::string image_path = "C:\\data\\cattle.jpg";

    //auto image = cv::imread(image_path);
    //std::cout << image.size() << std::endl;

    //// yuck...
    //std::vector<uchar> buf;
    //cv::imencode(".jpg", image, buf);
    //std::vector<char> image_data(buf.begin(), buf.end());

    std::ifstream image_file(image_path, std::ifstream::binary);

    //std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    std::stringstream ss;
    ss << image_file.rdbuf();

    std::string data = ss.str();

    std::string result = GraphRunner_Run("detection", "C:/data/detection_ssd.xml", data.c_str(), data.size());

    //std::cout << "graph open style: " << std::endl;
    //auto instance = GraphRunner_Open("detection", "C:/data/detection_ssd.xml");
    //std::thread t1(listen_task, instance);
    //for ( int i = 0; i < 10; i++ ) {
    //    GraphRunner_Queue(instance, "C:/data/cattle.jpg");
    //}

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    //GraphRunner_Queue(instance, "C:/data/cattle.jpg");


    //GraphRunner_Close(instance);

    //t1.join();
    return 0;
}
