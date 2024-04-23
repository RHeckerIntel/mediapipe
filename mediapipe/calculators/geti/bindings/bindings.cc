#include "bindings.h"
#include <fstream>
#include "graph_runner.h"

DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_type, const char* model_path) {
    auto runner= new geti::GraphRunner();

    runner->OpenGraph(std::string(graph_type), std::string(model_path));

    return runner;
}

DLLEXPORT const char* GraphRunner_Run(const char* graph_type, const char* model_path, const char* image_path){
    std::ifstream image_file(image_path, std::ifstream::binary);
    std::vector<char> image_data((std::istreambuf_iterator<char>(image_file)), std::istreambuf_iterator<char>());
    auto runner = geti::GraphRunner();
    runner.OpenGraph(graph_type, model_path);
    runner.Queue(image_data);
    static std::string result = runner.Get();
    std::cout << " before return" << result.size() << std::endl;
    return result.c_str();
}

DLLEXPORT void GraphRunner_Close(void* instance);
DLLEXPORT int GraphRunner_Get();
DLLEXPORT int GraphRunner_Queue(char* image_path);
