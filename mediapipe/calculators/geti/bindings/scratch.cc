#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>
#include <thread>

#include "bindings.h"

void callback(const char* data) {
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

  auto instance = GraphRunner_Open(graph.c_str());
  GraphRunner_OpenCamera(instance, "/dev/video0");

  auto thread = std::thread(GraphRunner_Listen,instance, callback);
  std::this_thread::sleep_for(5000ms);
  GraphRunner_Close(instance);
  thread.join();
  return 0;
}
