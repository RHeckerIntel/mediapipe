#ifndef DETECTION_SEGMENTATION_RESULT_CALCULATOR_H_
#define DETECTION_SEGMENTATION_RESULT_CALCULATOR_H_

#include <models/input_data.h>
#include <models/results.h>

#include "data_structures.h"
#include "../inference/geti_calculator_base.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/status.h"

namespace mediapipe {

// Outputs image overlaying the detection classification task chain results
//
// Input:
//  DETECTION - GetiDetectionResult
//  DETECTION_SEGMENTATIONS - vector of SegmentationResult
//
// Output:
//  DETECTION_SEGMENTATION_RESULT - Combination object
//

class DetectionSegmentationResultCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;
};

}  // namespace mediapipe

#endif  // DETECTION_SEGMENTATION_RESULT_CALCULATOR_H_
