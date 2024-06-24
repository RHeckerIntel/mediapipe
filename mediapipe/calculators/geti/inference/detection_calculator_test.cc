#include "detection_calculator.h"

#include <map>
#include <string>
#include <vector>

#include "../inference/test_utils.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/calculator_runner.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/port/gtest.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status_matchers.h"
#include "mediapipe/util/image_test_utils.h"

namespace mediapipe {
CalculatorGraphConfig graph_config =
    ParseTextProtoOrDie<CalculatorGraphConfig>(absl::Substitute(
        R"pb(
          input_stream: "input"
          input_side_packet: "model_path"
          input_side_packet: "device"
          output_stream: "output"
          node {
            calculator: "OpenVINOInferenceAdapterCalculator"
            input_side_packet: "MODEL_PATH:model_path"
            input_side_packet: "DEVICE:device"
            output_side_packet: "INFERENCE_ADAPTER:adapter"
          }
          node {
            calculator: "DetectionCalculator"
            input_side_packet: "INFERENCE_ADAPTER:adapter"
            input_stream: "IMAGE:input"
            output_stream: "INFERENCE_RESULT:output"
          }
        )pb"));

TEST(DetectionCalculatorTest, TestDetection) {
  const cv::Mat raw_image = cv::imread("/data/cattle.jpg");
  std::vector<Packet> output_packets;
  std::string model_path = "/data/geti/detection_atss.xml";

  std::map<std::string, mediapipe::Packet> inputSidePackets;
  inputSidePackets["model_path"] =
      mediapipe::MakePacket<std::string>(model_path)
          .At(mediapipe::Timestamp(0));
  inputSidePackets["device"] =
      mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));
  geti::RunGraph(mediapipe::MakePacket<cv::Mat>(raw_image), graph_config,
                 output_packets, inputSidePackets);
  ASSERT_EQ(1, output_packets.size());

  auto result = output_packets[0].Get<geti::InferenceResult>();
  auto detections = result.rectangles;

  ASSERT_EQ(detections.size(), 4);

  cv::Rect roi(0, 0, raw_image.cols, raw_image.rows);
  ASSERT_EQ(result.roi, roi);
  const auto &detection = detections[0];
  ASSERT_EQ(detection.labels[0].label.label_id, "653b84c34e88964031d81a48");

  const auto &cow_map = result.saliency_maps[0];
  ASSERT_EQ(cow_map.label.label_id, "653b84c34e88964031d81a47");

  const auto &sheep_map = result.saliency_maps[1];
  ASSERT_EQ(sheep_map.label.label_id, "653b84c34e88964031d81a48");
}

}  // namespace mediapipe
