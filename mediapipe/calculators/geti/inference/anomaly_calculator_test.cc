#include "anomaly_calculator.h"

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
#include "models/image_model.h"
#include "../utils/data_structures.h"

namespace mediapipe {

TEST(AnomalyCalculatorTest, TestImageAnomaly) {
  const cv::Mat raw_image = cv::imread("/data/cattle.jpg");
  const std::string model_path = "/data/geti/anomaly_classification_padim.xml";

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
              calculator: "AnomalyCalculator"
              input_side_packet: "INFERENCE_ADAPTER:adapter"
              input_stream: "IMAGE:input"
              output_stream: "INFERENCE_RESULT:output"
            }
          )pb"));
  std::vector<Packet> output_packets;

  std::map<std::string, mediapipe::Packet> inputSidePackets;
  inputSidePackets["model_path"] =
      mediapipe::MakePacket<std::string>(model_path)
          .At(mediapipe::Timestamp(0));
  inputSidePackets["device"] =
      mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));

  auto packet = mediapipe::MakePacket<cv::Mat>(raw_image);
  geti::RunGraph(packet, graph_config, output_packets, inputSidePackets);

  ASSERT_EQ(1, output_packets.size());

  auto &result = output_packets[0].Get<geti::InferenceResult>();
  cv::Rect roi(0, 0, raw_image.cols, raw_image.rows);
  ASSERT_EQ(result.roi, roi);
  ASSERT_EQ(1, result.rectangles.size());
  ASSERT_NEAR(0.699, result.rectangles[0].labels[0].probability, 0.01);
  ASSERT_EQ("Anomaly", result.rectangles[0].labels[0].label.label);
}
}  // namespace mediapipe
