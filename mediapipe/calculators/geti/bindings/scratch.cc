#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>
#include <thread>

#include "bindings.h"

void callback(const char*) {
  std::cout << "got package" << std::endl;
}

int main() {

  using namespace std::chrono_literals;
  std::cout << "Hello world" << std::endl;

  std::string graph_path = "mediapipe/calculators/geti/graphs/examples/test_graph.pbtxt";

  std::ifstream t(graph_path);

  std::stringstream buffer;
  buffer << t.rdbuf();
  std::string graph = buffer.str();

  std::cout << sizeof(size_t) << std::endl;
  std::cout << sizeof(long) << std::endl;

  const char* model_path = "/home/ronald/.local/share/intel.openvino.console/3c19fd52-bcab-44fa-8537-0426073a29c2/662aac23edcb02d8b632309a_model.xml";
  const char* model_typ = "detection";
  const char* output = "/home/ronald/.local/share/intel.openvino.console/3c19fd52-bcab-44fa-8537-0426073a29c2/662aac23edcb02d8b632309a.xml";
  SerializeModel(model_path, model_typ, output);


  //auto graph_runner = GraphRunner_Open(graph.c_str());
  //GraphRunner_Listen(graph_runner, callback);
  //GraphRunner_OpenCamera(graph.c_str(), "/dev/video0", callback);

  //std::cout << GraphRunner_Get(graph_runner) << std::endl;
  //std::cout << GraphRunner_Get(graph_runner) << std::endl;
  //std::cout << GraphRunner_Get(graph_runner) << std::endl;

  return 0;
}
