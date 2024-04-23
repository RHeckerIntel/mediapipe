#include "bindings.h"
#include <fstream>
#include "graph_runner.h"

DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_type, const char* model_path) {
    auto runner= new geti::GraphRunner();

    runner->OpenGraph(std::string(graph_type), std::string(model_path));

    return runner;
}

DLLEXPORT const char* GraphRunner_Run(const char* graph_type, const char* model_path, const char* image_data, const size_t data_length){
    //std::ifstream image_file(image_path, std::ifstream::binary);
    //std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    std::vector<char> image(image_data, image_data + data_length);
    auto runner = geti::GraphRunner();
    runner.OpenGraph(graph_type, model_path);
    runner.Queue(image);
    static std::string result = runner.Get();
    std::cout << " before return" << result.size() << std::endl;
    return result.c_str();
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
