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
constexpr char kInputStream[] = "input";
constexpr char kOutputStream[] = "output";
constexpr char kWindowName[] = "MediaPipe";
constexpr float kMicrosPerSecond = 1e6;

namespace {

absl::Status ProcessImage(std::unique_ptr<mediapipe::CalculatorGraph> graph, const std::string& image_path, const std::string& output_path) {
  LOG(INFO) << "Load the image.";
  const cv::Mat raw_image = cv::imread(image_path);
  cv::cvtColor(raw_image, raw_image, cv::COLOR_BGR2RGB); // this happens inside the ModelInferRequest

  LOG(INFO) << "Start running the calculator graph.";
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller output_image_poller,
                   graph->AddOutputStreamPoller(kOutputStream));

  LOG(INFO) << "Start running the calculator graph input.";

  MP_RETURN_IF_ERROR(graph->StartRun({}));

  // Send image packet into the graph.
  MP_RETURN_IF_ERROR(graph->AddPacketToInputStream(
      kInputStream,
      mediapipe::MakePacket<cv::Mat>(raw_image).At(mediapipe::Timestamp(0))));

  // Get the graph result packets, or stop if that fails.
  mediapipe::Packet output_image_packet;
  if (!output_image_poller.Next(&output_image_packet)) {
    return absl::UnknownError(
        "Failed to get packet from output stream 'output_image'.");
  }

  cv::Mat output_frame_mat = output_image_packet.Get<cv::Mat>();
  LOG(INFO) << "Saving image to file...";
  cv::imwrite(output_path, output_frame_mat);

  LOG(INFO) << "Shutting down.";
  MP_RETURN_IF_ERROR(graph->CloseInputStream(kInputStream));

  return graph->WaitUntilDone();
}

absl::Status RunMPPGraph(const std::string& graph_path, const std::string& image_path, const std::string& output_path) {
  std::string calculator_graph_config_contents;
    mediapipe::file::GetContents(graph_path, &calculator_graph_config_contents);

  mediapipe::CalculatorGraphConfig graph_config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(calculator_graph_config_contents);


  LOG(INFO) << "Initialize the calculator graph.";
  std::unique_ptr<mediapipe::CalculatorGraph> graph =
      absl::make_unique<mediapipe::CalculatorGraph>();
  MP_RETURN_IF_ERROR(graph->Initialize(graph_config));
  LOG(INFO) << "Success Initialize the calculator graph.";

  return ProcessImage(std::move(graph), image_path, output_path);
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 4) {
    throw std::runtime_error(std::string{"Usage: "} + argv[0] + " <path_to_graph> <path_to_image> <path_to_output_image>");
  }

  std::string graph_path = argv[1];
  std::string image_path = argv[2];
  std::string output_image_path = argv[3];

  google::InitGoogleLogging(argv[0]);

  auto status = RunMPPGraph(graph_path, image_path, output_image_path);
  if (status.ok()) {
    std::cout << "Succes! see: " << output_image_path << std::endl;
  } else {
    std::cout << "Failed to run the graph: " << status.message() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
