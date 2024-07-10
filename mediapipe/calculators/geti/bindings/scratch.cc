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


  auto graph_runner = GraphRunner_Open(graph.c_str());
  const char* data = "hello";
  GraphRunner_Queue(graph_runner, data, 5);

  while(true) {
    GraphRunner_Get(graph_runner);
    std::this_thread::sleep_for(2000ms);
  }

  return 0;
}
