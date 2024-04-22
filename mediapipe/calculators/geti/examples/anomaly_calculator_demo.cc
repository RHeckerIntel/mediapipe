/**
 *  INTEL CONFIDENTIAL
 *
 *  Copyright (C) 2023 Intel Corporation
 *
 *  This software and the related documents are Intel copyrighted materials, and
 * your use of them is governed by the express license under which they were
 * provided to you ("License"). Unless the License provides otherwise, you may
 * not use, modify, copy, publish, distribute, disclose or transmit this
 * software or the related documents without Intel's prior written permission.
 *
 *  This software and the related documents are provided as is, with no express
 * or implied warranties, other than those that are expressly stated in the
 * License.
 *
 */
#include <cstdlib>
#include <memory>
#include <opencv2/core.hpp>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/canonical_errors.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "../utils/data_structures.h"

constexpr char kSidePacket[] = "model_path";
constexpr char kDevice[] = "device";
constexpr char kInputStream[] = "input_image";
constexpr char kOutputStream[] = "output_image";
constexpr char kWindowName[] = "MediaPipe";
constexpr float kMicrosPerSecond = 1e6;

ABSL_FLAG(std::string, input_image_path, "C:/data/cattle.jpg",
          "Full path of image to load. "
          "If not provided, nothing will run.");
ABSL_FLAG(std::string, output_image_path, "C:/data/mp_dep_output.jpg",
          "Full path of where to save image result (.jpg only). "
          "If not provided, show result in a window.");
ABSL_FLAG(std::string, graph_config_path,
          "mediapipe/calculators/geti/graphs/examples/mapi_anomaly_calculator.pbtxt",
          "Full path to the graph description file.");
ABSL_FLAG(std::string, model_xml_path,
          "C:/data/detection_ssd.xml",
          "Full path to the model xml file.");

namespace {

absl::Status ProcessImage(std::unique_ptr<mediapipe::CalculatorGraph> graph) {
  std::cout << "Hello world" << std::endl;
  LOG(INFO) << "Load the image.";
  const cv::Mat raw_image = cv::imread(absl::GetFlag(FLAGS_input_image_path));

  std::cout << "img: " << raw_image.size() << std::endl;

  LOG(INFO) << "Start running the calculator graph.";
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller output_image_poller,
                   graph->AddOutputStreamPoller(kOutputStream));

  LOG(INFO) << "Start running the calculator graph input.";

  std::map<std::string, mediapipe::Packet> inputSidePackets;
  inputSidePackets[kSidePacket] =
      mediapipe::MakePacket<std::string>(absl::GetFlag(FLAGS_model_xml_path))
          .At(mediapipe::Timestamp(0));
  inputSidePackets[kDevice] =
      mediapipe::MakePacket<std::string>("AUTO").At(mediapipe::Timestamp(0));
  MP_RETURN_IF_ERROR(graph->StartRun(inputSidePackets));

  // Send image packet into the graph.
  MP_RETURN_IF_ERROR(graph->AddPacketToInputStream(
      kInputStream,
      mediapipe::MakePacket<cv::Mat>(raw_image).At(mediapipe::Timestamp(0))));

  std::cout << "so far so good" << std::endl;

  // Get the graph result packets, or stop if that fails.
  mediapipe::Packet output_image_packet;
  if (!output_image_poller.Next(&output_image_packet)) {
    return absl::UnknownError(
        "Failed to get packet from output stream 'output_image'.");
  }

  std::cout << "got a pacakge?" << std::endl;

  cv::Mat output_frame_mat = output_image_packet.Get<cv::Mat>();
  const bool save_image = !absl::GetFlag(FLAGS_output_image_path).empty();
  std::cout << save_image << "---------------------\n";
  if (save_image) {
    LOG(INFO) << "Saving image to file...";
    cv::imwrite(absl::GetFlag(FLAGS_output_image_path), output_frame_mat);
  } else {
    cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
    cv::imshow(kWindowName, output_frame_mat);
    // Press any key to exit.
    cv::waitKey(0);
  }

  LOG(INFO) << "Shutting down.";
  MP_RETURN_IF_ERROR(graph->CloseInputStream(kInputStream));

  return graph->WaitUntilDone();
}

absl::Status RunMPPGraph() {
  std::string calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig graph_config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(absl::Substitute(
        R"pb(
          input_stream : "input_image"
          input_side_packet : "model_path"
          input_side_packet : "device"
          output_stream : "output_image"

          node {
          calculator : "OpenVINOInferenceAdapterCalculator"
          input_side_packet : "MODEL_PATH:model_path"
          input_side_packet : "DEVICE:device"
          output_side_packet : "INFERENCE_ADAPTER:adapter"
          }

          node {
          calculator : "DetectionCalculator"
          input_side_packet : "INFERENCE_ADAPTER:adapter"
          input_stream : "IMAGE:input_image"
          output_stream: "INFERENCE_RESULT:result"
          }

          node {
          calculator : "OverlayCalculator"
          input_stream : "IMAGE:input_image"
          input_stream : "INFERENCE_RESULT:result"
          output_stream : "IMAGE:output_image"
          }
        )pb"));


  LOG(INFO) << "Initialize the calculator graph.";
  std::unique_ptr<mediapipe::CalculatorGraph> graph =
      absl::make_unique<mediapipe::CalculatorGraph>();
  MP_RETURN_IF_ERROR(graph->Initialize(graph_config));
  LOG(INFO) << "Success Initialize the calculator graph.";

  const bool load_image = !absl::GetFlag(FLAGS_input_image_path).empty();
  if (load_image) {
    return ProcessImage(std::move(graph));
  } else {
    return absl::InvalidArgumentError("Missing image file.");
  }

  return absl::InvalidArgumentError("Missing image file.");
}

}  // namespace

int main(int argc, char** argv) {
  std::cout << "Hello world" << std::endl;
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);
  absl::Status run_status = RunMPPGraph();
  if (!run_status.ok()) {
    std::cout << "Failed to run the graph: " << run_status.message() << std::endl;
    return EXIT_FAILURE;
  } else {
    std::cout << "Success!" << std::endl;
  }
  return EXIT_SUCCESS;
}
