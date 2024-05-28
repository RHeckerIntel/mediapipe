#include "bindings.h"
#include <iostream>
#include <fstream>
#include "graph_runner.h"

#include <models/detection_model.h>
#include <models/classification_model.h>
#include <models/segmentation_model.h>
#include <models/instance_segmentation.h>
#include <models/anomaly_model.h>
#include <stdexcept>

DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_content) {
    auto runner= new geti::GraphRunner();

    std::cout << "opening graph: " << std::endl;
    std::cout << graph_content << std::endl;
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
    auto result = new std::string(reinterpret_cast<geti::GraphRunner*>(instance)->Get());
    return result->c_str();
}

DLLEXPORT void GraphRunner_Queue(CGraphRunner instance, const char* image_data, const size_t data_length) {
    //std::ifstream image_file(image_path, std::ifstream::binary);
    //std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    std::vector<char> image(image_data, image_data + data_length);
    reinterpret_cast<geti::GraphRunner*>(instance)->Queue(image);
}

DLLEXPORT void GraphRunner_QueueText(CGraphRunner instance, const char* input) {
    reinterpret_cast<geti::GraphRunner*>(instance)->Queue(input);
}

DLLEXPORT void GraphRunner_Listen(CGraphRunner instance, CallbackFunction callback) {
    auto lambda_callback = [callback](const std::string& message) {
        callback(message.c_str());
    };
    reinterpret_cast<geti::GraphRunner*>(instance)->Listen(lambda_callback);
}

DLLEXPORT void SerializeModel(const char* model_path, const char* model_type, const char* output_filename) {
    std::string model_type_str(model_type);
    std::unique_ptr<ModelBase> model;
    std::cout << "serializing " << model_type_str << std::endl;
    std::cout << "input " << model_path << std::endl;
    std::cout << "output " << output_filename << std::endl;

    if (model_type_str == "detection") {
        model = DetectionModel::create_model(model_path);
    } else if (model_type_str == "classification") {
        model = ClassificationModel::create_model(model_path);
    } else if (model_type_str == "segmentation") {
        model = SegmentationModel::create_model(model_path);
    } else if (model_type_str == "rotated_detection") {
        model = MaskRCNNModel::create_model(model_path);
    } else if (model_type_str == "instance_segmentation") {
        model = MaskRCNNModel::create_model(model_path);
    } else if (model_type_str == "anomaly_detection") {
        model = AnomalyModel::create_model(model_path);
    } else if (model_type_str == "anomaly_classification") {
        model = AnomalyModel::create_model(model_path);
    } else if (model_type_str == "anomaly_segmentation") {
        model = AnomalyModel::create_model(model_path);
    } else {
        throw std::runtime_error("Model type not available");
    }

    const std::shared_ptr<ov::Model>& ov_model = model->getModel();
    ov::serialize(ov_model, output_filename);
}
