#ifndef VIDEO_INPUT_CALCULATOR_H_
#define VIDEO_INPUT_CALCULATOR_H_

#include "mediapipe/calculators/geti/inference/geti_calculator_base.h"
#include "mediapipe/framework/calculator_framework.h"

namespace mediapipe {

// Runs detection inference on the provided image and OpenVINO model.
//
// Input:
//
// Output:
//  IMAGE - cv::Mat from video
//

class VideoInputCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status GetiOpen(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;
private:
  Timestamp loop_internal_timestamp_ = Timestamp(0);
};

}  // namespace mediapipe


#endif // VIDEO_INPUT_CALCULATOR_H_
