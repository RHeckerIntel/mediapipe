#include "bindings.h"
#include <iostream>
#include <fstream>
#include "graph_runner.h"

#include <models/detection_model.h>
#include <models/classification_model.h>
#include <models/segmentation_model.h>

DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_content) {
    auto runner= new geti::GraphRunner();

    runner->OpenGraph(std::string(graph_content));

    return runner;
}

DLLEXPORT const char* GraphRunner_Run(const char* graph_content, const char* image_data, const size_t data_length){
    //std::ifstream image_file(image_path, std::ifstream::binary);
    //std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    std::vector<char> image(image_data, image_data + data_length);
    std::cout << "data_length: " << data_length  << std::endl;
    std::cout << "image_length: " << image.size()  << std::endl;
    auto runner = geti::GraphRunner();
    runner.OpenGraph(graph_content);
    runner.Queue(image);
    auto result = new std::string(runner.Get());
    std::cout << " before return" << result->size() << std::endl;
    return result->c_str();
}

DLLEXPORT void GraphRunner_Close(void* instance) {
    auto runner = reinterpret_cast<geti::GraphRunner*>(instance);
    runner->Stop();
    delete runner;
}


DLLEXPORT const char* GraphRunner_Get(CGraphRunner instance) {
    static std::string result = reinterpret_cast<geti::GraphRunner*>(instance)->Get();
    return result.c_str();
}

DLLEXPORT void GraphRunner_Queue(CGraphRunner instance, const char* image_data, const size_t data_length) {
    //std::ifstream image_file(image_path, std::ifstream::binary);
    //std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    std::vector<char> image(image_data, image_data + data_length);
    reinterpret_cast<geti::GraphRunner*>(instance)->Queue(image);
}

DLLEXPORT void GraphRunner_Listen(CGraphRunner instance, CallbackFunction callback) {
    auto lambda_callback = [callback](const std::string& message) {
        callback(message.c_str());
    };
    reinterpret_cast<geti::GraphRunner*>(instance)->Listen(lambda_callback);
}

DLLEXPORT void SerializeModel(const char* model_path, const char* model_type, const char* output_filename) {
    std::string model_type_str(model_type);
    // dont care about duplication for now.
    if (model_type_str == "detection") {
        auto model = DetectionModel::create_model(model_path);
        const std::shared_ptr<ov::Model>& ov_model = model->getModel();
        ov::serialize(ov_model, output_filename);
    }
    if (model_type_str == "classification") {
        auto model = ClassificationModel::create_model(model_path);
        const std::shared_ptr<ov::Model>& ov_model = model->getModel();
        ov::serialize(ov_model, output_filename);
    }
    if (model_type_str == "segmentation") {
        auto model = SegmentationModel::create_model(model_path);
        const std::shared_ptr<ov::Model>& ov_model = model->getModel();
        ov::serialize(ov_model, output_filename);
    }
}
