#ifndef APP_SERIALIZATION_CALCULATOR_H_
#define APP_SERIALIZATION_CALCULATOR_H_

#include <memory>

#include "../inference/geti_calculator_base.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/packet.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/opencv_imgcodecs_inc.h"
#include "mediapipe/framework/port/status.h"
#include "result_serialization.h"

#include "mediapipe/calculators/geti/serialization/app_serialization_calculator.pb.h"

namespace mediapipe {

// Serialize the output detections to json
//
// Input side packet:
//  RESULT - Result that has serialization implementation
//
// Output side packet:
//  RESPONSE - JSON String
//

class AppSerializationCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

  private:
    bool output_overlay = false;
    bool output_json = false;
    bool output_csv = false;
};

}

#endif // APP_SERIALIZATION_CALCULATOR_H_
