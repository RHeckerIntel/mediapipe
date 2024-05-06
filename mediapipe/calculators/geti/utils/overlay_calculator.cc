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
#include "../serialization/result_serialization.h"

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

  const auto &options =
      cc->Options<OverlayCalculatorOptions>();
  for (auto &label: options.labels()) {
    label_definitions.push_back(geti::ProjectLabel{
      label.id(),
      label.name(),
      geti::hex_to_color(label.color()),
      label.is_empty()});
  }

  if (options.has_font_size()) {
    drawOptions.fontSize = options.font_size();
  }

  if (options.has_opacity()) {
    drawOptions.opacity = options.opacity();
  }

  if (options.stroke_width()) {
    drawOptions.strokeWidth = options.stroke_width();
  }


  BLFontFace face;
  const char fontName[] = "/home/rhecker/Projects/blend2d_workspace/app/intelone-text-regular.ttf";
  BLResult result = face.createFromFile(fontName);
  if (result != BL_SUCCESS) {
      printf("Failed to load a font (err=%u)\n", result);
  }
  font.createFromFace(face, drawOptions.fontSize);




  for (auto label: label_definitions) {
    std::cout << label.name << std::endl;
  }

  return absl::OkStatus();
}

absl::Status OverlayCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "DetectionOverlayCalculator::GetiProcess()";
  if (cc->Inputs().Tag("IMAGE").IsEmpty()) {
    return absl::OkStatus();
  }

  // Get inputs
  const cv::Mat &input_image = cc->Inputs().Tag("IMAGE").Get<cv::Mat>();
  auto result =
      cc->Inputs().Tag("INFERENCE_RESULT").Get<geti::InferenceResult>();

  // Apply results
  cv::Mat cv_image;
  cv::cvtColor(input_image, cv_image, cv::COLOR_RGB2BGRA);

  BLImage img;
  img.createFromData(cv_image.cols, cv_image.rows, BLFormat::BL_FORMAT_XRGB32, cv_image.data, cv_image.step);
  BLContext ctx(img);
  ctx.setStrokeWidth(drawOptions.strokeWidth);

  for (auto &detection : result.rectangles) {
    draw_rect_prediction(ctx, font, detection, label_definitions, drawOptions);
  }

  for (auto &polygon : result.polygons) {
    draw_polygon_prediction(ctx, font, polygon, label_definitions, drawOptions);
  }

  for (auto &rotated_rect : result.rotated_rectangles) {
    draw_rotated_rect_prediction(ctx, font, rotated_rect, label_definitions, drawOptions);
  }

  ctx.end();

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
