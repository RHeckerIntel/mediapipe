#include "video_input_calculator.h"
#include "mediapipe/framework/port/opencv_videoio_inc.h"
#include "mediapipe/framework/tool/status_util.h"

namespace mediapipe {

absl::Status VideoInputCalculator::GetContract(CalculatorContract *cc) {
  LOG(INFO) << "VideoInputCalculator::GetContract()";
  cc->Outputs().Tag("IMAGE").Set<cv::Mat>();
  return absl::OkStatus();
}

absl::Status VideoInputCalculator::GetiOpen(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::Open()";
  return absl::OkStatus();
}

absl::Status VideoInputCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::GetContract()";

  cv::VideoCapture cap;
  cv::Mat frame;
  int deviceID = 0;
  cap.open(deviceID);
  if (!cap.isOpened()) {
    LOG(ERROR) << "ERROR! Unable to open camera\n";
    return absl::UnknownError("ERROR! Unable to open camera\n");
  }

  size_t maxFrames = 10;

  Timestamp last_timestamp = loop_internal_timestamp_;
  for (size_t i = 0; i < maxFrames; i++) {
    cap.read(frame);
    cc->Outputs().Tag("IMAGE").AddPacket(
        MakePacket<cv::Mat>(frame.clone()).At(loop_internal_timestamp_));
    ++loop_internal_timestamp_;
  }

  cap.release();
  return tool::StatusStop();
  //return absl::OkStatus();
}

absl::Status VideoInputCalculator::Close(CalculatorContext *cc) {
  LOG(INFO) << "VideoInputCalculator::Close()";
  return absl::OkStatus();
}

REGISTER_CALCULATOR(VideoInputCalculator);

} // namespace mediapipe
