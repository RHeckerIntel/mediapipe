#ifndef GETI_CALCULATOR_BASE_H
#define GETI_CALCULATOR_BASE_H

#include <iostream>

#include "mediapipe/framework/calculator_framework.h"

namespace mediapipe {

class GetiCalculatorBase : public CalculatorBase {
 public:
  absl::Status Process(CalculatorContext* cc) override {
    try {
      return GetiProcess(cc);
    } catch (const std::exception& e) {
      std::cout << "Caught exception with message: " << e.what() << std::endl;
      RET_CHECK(false);
    } catch (...) {
      std::cout << "Caught unknown exception" << std::endl;
      RET_CHECK(false);
    }
  }
  virtual absl::Status GetiProcess(CalculatorContext* cc) = 0;
};

}  // namespace mediapipe

#endif  // GETI_CALCULATOR_BASE_H
