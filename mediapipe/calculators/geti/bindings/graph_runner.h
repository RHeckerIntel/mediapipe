#ifndef BINDINGS_BINDINGS_H_
#define BINDINGS_BINDINGS_H_

#include <models/detection_model.h>
#include <models/input_data.h>
#include <models/results.h>
#include <functional>

#include <vector>
#include <memory>
#include <stdexcept>

#include "mediapipe/calculators/geti/inference/kserve.h"
#include "mediapipe/framework/calculator_framework.h"
// #include "node/graphs.h"
#include "mediapipe/calculators/geti/serialization/result_serialization.h"

constexpr float kMicrosPerSecond = 1e6;

namespace geti {
class GraphRunner  {
 public:
  GraphRunner(): graph(std::make_shared<mediapipe::CalculatorGraph>()) {}
  void OpenGraph(const char* graph_content);
  std::string Get();
  void Listen(const std::function<void(const std::string&)> callback);
  void Queue(const std::vector<char>& image_data);
  void Queue(const std::string& input);
  void Stop();

  static void SetupLogging(const char* filename);
 private:

  // Data stored in these variables is unique to each instance of the add-on.
  int64 timestamp = 0;
  std::shared_ptr<mediapipe::OutputStreamPoller> poller;
  std::shared_ptr<mediapipe::CalculatorGraph> graph;
  bool running = false;
};
}

#endif // BINDINGS_BINDINGS_H_
