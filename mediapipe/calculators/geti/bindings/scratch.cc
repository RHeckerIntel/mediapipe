#include "bindings.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <thread>

#include "mediapipe/calculators/geti/bindings/graph_runner.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"

/*
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_videoio_inc.h"


void video_stuff() {
  cv::VideoCapture cap;
  cv::Mat frame;

  int deviceID = 0;
  cap.open(deviceID);
  if (!cap.isOpened()) {
    std::cerr << "ERROR! Unable to open camera\n";
    return;
  }

  cap.read(frame);
  cv::imwrite("/data/output.png", frame);
  cap.release();
}
*/

static const char *graph_content = R"pb(
output_stream : "output"

node {
    calculator : "OpenVINOInferenceAdapterCalculator"
    output_side_packet : "INFERENCE_ADAPTER:adapter"
    node_options: {
        [type.googleapis.com/mediapipe.OpenVINOInferenceAdapterCalculatorOptions] {
            model_path: "/data/omz_models/intel/face-detection-retail-0004/face-detection-retail-0004.xml"
        }
    }
}

node {
    calculator : "VideoInputCalculator"
    output_stream: "IMAGE:input_image"
}

node {
    calculator : "DetectionCalculator"
    input_side_packet : "INFERENCE_ADAPTER:adapter"
    input_stream : "IMAGE:input_image"
    output_stream: "INFERENCE_RESULT:inference_detections"
}

node {
    calculator : "OverlayCalculator"
    input_stream : "IMAGE:input_image"
    input_stream : "INFERENCE_RESULT:inference_detections"
    output_stream: "IMAGE:output"
    node_options: {
        [type.googleapis.com/mediapipe.OverlayCalculatorOptions] {
            labels: {
                id: "face"
                name: "face"
                color: "#f7dab3ff"
                is_empty: false
            }

            stroke_width: 2
            opacity: 0.4
            font_size: 1.0
        }
    }
}
)pb";

int main() {
  std::cout << "scratch" << std::endl;

  std::shared_ptr<mediapipe::OutputStreamPoller> poller;
  std::shared_ptr<mediapipe::CalculatorGraph> graph;

  mediapipe::CalculatorGraphConfig graph_config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          absl::Substitute(graph_content));
  graph = std::make_shared<mediapipe::CalculatorGraph>(graph_config);

  poller = std::unique_ptr<mediapipe::OutputStreamPoller>(
      new mediapipe::OutputStreamPoller(
          graph->AddOutputStreamPoller("output").value()));
  // graph->Initialize(graph_config);
  graph->StartRun({});

  mediapipe::Packet output_packet;
  for (int i = 0; i < 20; i++) {
    std::string path = "/data/video/output_" + std::to_string(i) + ".png";
    if (poller->Next(&output_packet)) {
        std::cout << path << std::endl;
        cv::Mat output_image = output_packet.Get<cv::Mat>();
        cv::cvtColor(output_image, output_image, cv::COLOR_BGR2RGB);
        cv::imwrite(path, output_image);
    }
  }

  return 0;
}
