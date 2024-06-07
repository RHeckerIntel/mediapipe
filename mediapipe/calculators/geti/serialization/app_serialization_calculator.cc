#include "app_serialization_calculator.h"
#include "../inference/utils.h"
#include "nlohmann/json.hpp"
#include "result_serialization.h"
#include "csv_serialization.h"
#include "../utils/data_structures.h"
#include "utils/ocv_common.hpp"


namespace mediapipe {

absl::Status AppSerializationCalculator::GetContract(CalculatorContract *cc) {
  LOG(INFO) << "AppSerializationCalculator::GetContract()";
  cc->Inputs().Tag("OVERLAY").Set<cv::Mat>().Optional();
  cc->Inputs().Tag("CSV").Set<bool>().Optional();
  cc->Inputs().Tag("JSON").Set<bool>().Optional();
  cc->Inputs().Tag("INFERENCE_RESULT").Set<geti::InferenceResult>();

  cc->Outputs().Tag("RESULT").Set<std::string>();


  return absl::OkStatus();
}

absl::Status AppSerializationCalculator::GetiOpen(CalculatorContext *cc) {
  LOG(INFO) << "AppSerializationCalculator::Open()";

  const auto &options =
      cc->Options<AppSerializationCalculatorOptions>();

  if (options.has_overlay()) {
      output_overlay = options.overlay();
  }

  if (options.has_csv()) {
      output_csv = options.csv();
  }

  if (options.has_json()) {
      output_json = options.json();
  }

  return absl::OkStatus();
}

absl::Status AppSerializationCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "AppSerializationCalculator::GetiProcess()";
  auto result = cc->Inputs().Tag("INFERENCE_RESULT").Get<geti::InferenceResult>();

  bool include_xai = false;

  if (!include_xai) {
    result.saliency_maps.clear();
  }
  nlohmann::json output = {};
  if (output_json) {
    output["json"] = result;
    if (!include_xai) {
        output["json"].erase("maps");  // Remove empty array added by serializer.
    }
  }

  if (output_csv) {
    output["csv"] = geti::csv_serialize(result);
  }

  if (output_overlay && cc->Inputs().HasTag("OVERLAY")) {
      //Base 64 encode the overlay into the json
    cv::Mat overlay = cc->Inputs().Tag("OVERLAY").Get<cv::Mat>();
    cv::cvtColor(overlay, overlay, cv::COLOR_BGR2RGB);
    output["overlay"] = geti::base64_encode_mat(overlay);
  }

  cc->Outputs()
      .Tag("RESULT")
      .AddPacket(MakePacket<std::string>(output.dump())
                     .At(cc->InputTimestamp()));
  return absl::OkStatus();
}
absl::Status AppSerializationCalculator::Close(CalculatorContext *cc) {
  LOG(INFO) << "AppSerializationCalculator::Close()";
  return absl::OkStatus();
}

REGISTER_CALCULATOR(AppSerializationCalculator);

}  // namespace mediapipe
