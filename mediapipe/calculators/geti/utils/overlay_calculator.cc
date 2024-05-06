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
 */
#include "overlay_calculator.h"

#include "overlay.h"

namespace mediapipe {

absl::Status OverlayCalculator::GetContract(CalculatorContract *cc) {
  LOG(INFO) << "DetectionOverlayCalculator::GetContract()";
  cc->Inputs().Tag("IMAGE").Set<cv::Mat>();
  cc->Inputs().Tag("INFERENCE_RESULT").Set<geti::InferenceResult>();
  cc->Outputs().Tag("IMAGE").Set<cv::Mat>();

  return absl::OkStatus();
}

absl::Status OverlayCalculator::Open(CalculatorContext *cc) {
  LOG(INFO) << "DetectionOverlayCalculator::Open()";
  cc->SetOffset(TimestampDiff(0));
  BLFontFace face;
  const char fontName[] = "/home/rhecker/Projects/blend2d_workspace/app/intelone-text-regular.ttf";
  BLResult result = face.createFromFile(fontName);
  if (result != BL_SUCCESS) {
      printf("Failed to load a font (err=%u)\n", result);
  }
  font.createFromFace(face, 12.0f);

  return absl::OkStatus();
}

absl::Status OverlayCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "DetectionOverlayCalculator::GetiProcess()";
  if (cc->Inputs().Tag("IMAGE").IsEmpty()) {
    return absl::OkStatus();
  }

  std::vector<geti::ProjectLabel> label_definitions = geti::read_file("/home/rhecker/Projects/blend2d_workspace/app/project.json")["tasks"][0]["labels"];
  // Get inputs
  const cv::Mat &input_image = cc->Inputs().Tag("IMAGE").Get<cv::Mat>();
  const auto result =
      cc->Inputs().Tag("INFERENCE_RESULT").Get<geti::InferenceResult>();

  // Apply results
  cv::Mat cv_image;
  cv::cvtColor(input_image, cv_image, cv::COLOR_RGB2BGRA);

  BLImage img;
  img.createFromData(cv_image.cols, cv_image.rows, BLFormat::BL_FORMAT_XRGB32, cv_image.data, cv_image.step);
  BLContext ctx(img);
  ctx.setStrokeWidth(2);

  for (auto &detection : result.rectangles) {
    draw_rect_prediction(ctx, font, detection, label_definitions);
  }

  for (auto &obj : result.polygons) {
  }

  for (auto &obj : result.rotated_rectangles) {
  }

  cc->Outputs().Tag("IMAGE").AddPacket(
      MakePacket<cv::Mat>(cv_image).At(cc->InputTimestamp()));

  return absl::OkStatus();
}

absl::Status OverlayCalculator::Close(CalculatorContext *cc) {
  LOG(INFO) << "DetectionOverlayCalculator::Close()";
  return absl::OkStatus();
}

REGISTER_CALCULATOR(OverlayCalculator);

}  // namespace mediapipe
