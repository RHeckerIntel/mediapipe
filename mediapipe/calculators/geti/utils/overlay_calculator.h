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

#ifndef OVERLAY_CALCULATOR_H
#define OVERLAY_CALCULATOR_H

#include <models/input_data.h>
#include <models/results.h>

#include <vector>

#include "overlay.h"
#include "data_structures.h"
#include "../inference/geti_calculator_base.h"
#include "../serialization/project_serialization.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/calculators/geti/utils/overlay_calculator.pb.h"

#include <blend2d.h>

namespace mediapipe {

// Runs detection inference on the provided image and OpenVINO model.
//
// Input:
//  IMAGE - cv::Mat
//  INFERENCE_RESULT - geti::InferenceResult
//
// Output:
//  IMAGE - cv::Mat, Input image with applied detection bounding boxes
//
//

class OverlayCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

 private:
  BLFont font;
  std::vector<geti::ProjectLabel> label_definitions = {};
  DrawOptions drawOptions = {2.0, 0.4, 12.0};
};

}  // namespace mediapipe

#endif  // DETECTION_OVERLAY_CALCULATOR_H
