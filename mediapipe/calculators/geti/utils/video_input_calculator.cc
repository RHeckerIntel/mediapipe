#include "video_input_calculator.h"
#include "mediapipe/framework/tool/status_util.h"

namespace mediapipe {

absl::Status VideoInputCalculator::GetContract(CalculatorContract *cc) {
  LOG(INFO) << "VideoInputCalculator::GetContract()";
  cc->Inputs().Tag("INIT").SetAny().Optional();
  cc->Inputs().Tag("FINISHED").SetAny().Optional();
  cc->Outputs().Tag("IMAGE").Set<cv::Mat>();
  return absl::OkStatus();
}

absl::Status VideoInputCalculator::GetiOpen(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::Open()";
  const auto &options =
      cc->Options<VideoInputOptions>();

  device = options.device();

  cap.open(device);
  if (!cap.isOpened()) {
    LOG(ERROR) << "ERROR! Unable to open camera\n";
    return absl::UnknownError("ERROR! Unable to open camera\n");
  }


  return absl::OkStatus();
}

absl::Status VideoInputCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::GetiProcess()";

  cap.read(frame);
  std::cout << frame.rows << std::endl;
  cc->Outputs().Tag("IMAGE").AddPacket(
      MakePacket<cv::Mat>(frame.clone()).At(cc->InputTimestamp().NextAllowedInStream()));

  return absl::OkStatus();
}

absl::Status VideoInputCalculator::Close(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::Close()";
  cap.release();
  return absl::OkStatus();
}

REGISTER_CALCULATOR(VideoInputCalculator);

} // namespace mediapipe
