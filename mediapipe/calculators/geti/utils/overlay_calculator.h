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
  absl::Status GetiOpen(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

 private:
  BLFontFace face;
  std::vector<geti::ProjectLabel> label_definitions = {};
  DrawOptions drawOptions = {2.0, 0.4, 1.0};
};

}  // namespace mediapipe

#endif  // DETECTION_OVERLAY_CALCULATOR_H
