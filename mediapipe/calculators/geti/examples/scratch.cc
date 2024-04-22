#include <iostream>
#include <adapters/openvino_adapter.h>

#include <models/detection_model.h>
#include <models/input_data.h>
#include <models/results.h>


int main() {
    std::cout << "hello world" << std::endl;
    //auto core = ov::Core();

    //auto model = DetectionModel::create_model("C:/data/openvino.xml"); // works with SSD models. Download it using Python Model API
    return 0;
}
