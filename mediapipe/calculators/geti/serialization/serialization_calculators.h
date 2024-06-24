#ifndef SERIALIZATION_CALCULATOR_H_
#define SERIALIZATION_CALCULATOR_H_

#include <models/results.h>

#include <memory>

#include "../inference/geti_calculator_base.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/packet.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/opencv_imgcodecs_inc.h"
#include "mediapipe/framework/port/status.h"
#include "result_serialization.h"

namespace mediapipe {

// Serialize the output detections to a KFSResponse
//
// Input side packet:
//  RESULT - Result that has serialization implementation
//
// Output side packet:
//  RESPONSE - KFSResponse
//

class SerializationCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;
};

using DetectionSerializationCalculator = SerializationCalculator;
using DetectionSerializationCalculator = SerializationCalculator;
using RotatedDetectionSerializationCalculator = SerializationCalculator;
using ClassificationSerializationCalculator = SerializationCalculator;
using DetectionClassificationSerializationCalculator = SerializationCalculator;
using DetectionSegmentationSerializationCalculator = SerializationCalculator;
using SegmentationSerializationCalculator = SerializationCalculator;
using AnomalySerializationCalculator = SerializationCalculator;

}  // namespace mediapipe

#endif  // SERIALIZATION_CALCULATOR_H_
