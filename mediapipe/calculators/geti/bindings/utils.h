#ifndef BINDINGS_UTILS_H_
#define BINDINGS_UTILS_H_

#include "mediapipe/framework/calculator_framework.h"

#include "graphs.h"

namespace geti {
static const mediapipe::CalculatorGraphConfig& get_graph_config_by_task(const std::string& task_name) {

  if (task_name == "detection") {
    return geti::detection_graph_config;
  }
  std::cout << "no graphs available..." << std::endl;

  throw std::runtime_error("No graph available by name: '" + task_name + "'");
}
}


#endif // UTILS_H_
