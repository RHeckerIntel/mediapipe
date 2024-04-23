#ifndef GRAPHS_H_
#define GRAPHS_H_

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/parse_text_proto.h"

namespace geti {
  static mediapipe::CalculatorGraphConfig detection_graph_config =
    mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(absl::Substitute(
        R"pb(
          input_stream : "input"
          input_side_packet : "model_path"
          input_side_packet : "device"
          output_stream : "output"

          node {
          calculator : "OpenVINOInferenceAdapterCalculator"
          input_side_packet : "MODEL_PATH:model_path"
          input_side_packet : "DEVICE:device"
          output_side_packet : "INFERENCE_ADAPTER:adapter"
          }

          node {
          calculator : "DetectionCalculator"
          input_side_packet : "INFERENCE_ADAPTER:adapter"
          input_stream : "IMAGE:input"
          output_stream: "INFERENCE_RESULT:result"
          }

          node {
          calculator : "OverlayCalculator"
          input_stream : "IMAGE:input"
          input_stream : "INFERENCE_RESULT:result"
          output_stream : "IMAGE:output"
          }
        )pb"));
}


#endif // GRAPHS_H_
