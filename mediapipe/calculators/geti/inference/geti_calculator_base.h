/**
 *  INTEL CONFIDENTIAL
 *
 *  Copyright (C) 2023 Intel Corporation
 *
 *  This software and the related documents are Intel copyrighted materials, and
 * your use of them is governed by the express license under which they were
 * provided to you ("License"). Unless the License provides otherwise, you may
 * not use, modify, copy, publish, distribute, disclose or transmit this
 * software or the related documents without Intel's prior written permission.
 *
 *  This software and the related documents are provided as is, with no express
 * or implied warranties, other than those that are expressly stated in the
 * License.
 */
#ifndef GETI_CALCULATOR_BASE_H
#define GETI_CALCULATOR_BASE_H

#include <iostream>

#include "mediapipe/framework/calculator_framework.h"

namespace mediapipe {

class GetiCalculatorBase : public CalculatorBase {
 public:
  absl::Status Open(CalculatorContext* cc) override {
    try {
      return GetiOpen(cc);
    } catch (const std::exception& e) {
      LOG(ERROR) << "Caught exception with message: " << e.what();
      RET_CHECK(false);
    } catch (...) {
      LOG(ERROR) << "Caught unknown exception";
      RET_CHECK(false);
    }
  }

  absl::Status Process(CalculatorContext* cc) override {
    try {
      return GetiProcess(cc);
    } catch (const std::exception& e) {
      LOG(ERROR) << "Caught exception with message: " << e.what();
      RET_CHECK(false);
    } catch (...) {
      LOG(ERROR) << "Caught unknown exception";
      RET_CHECK(false);
    }
  }
  virtual absl::Status GetiProcess(CalculatorContext* cc) = 0;
  virtual absl::Status GetiOpen(CalculatorContext* cc) = 0;
};

}  // namespace mediapipe

#endif  // GETI_CALCULATOR_BASE_H
