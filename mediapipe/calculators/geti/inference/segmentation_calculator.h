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
#ifndef SEGMENTATION_CALCULATOR_H
#define SEGMENTATION_CALCULATOR_H

#include <models/input_data.h>
#include <models/results.h>
#include <models/segmentation_model.h>

#include <memory>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/status.h"
#include "utils/data_structures.h"

namespace mediapipe {

// Runs segmentation inference on the provided image and OpenVINO model.
//
// Input:
//  IMAGE - cv::Mat
//
// Output:
//  RESULT - SegmentationResult
//
// Input side packet:
//  INFERENCE_ADAPTER - std::shared_ptr<InferenceAdapter>
//

class SegmentationCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status Process(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

 private:
  std::shared_ptr<InferenceAdapter> ia;
  std::unique_ptr<SegmentationModel> model;
  std::vector<Label> labels;
  std::map<std::string, Label> labels_map;
};

}  // namespace mediapipe

#endif  // SEGMENTATION_CALCULATOR_H
