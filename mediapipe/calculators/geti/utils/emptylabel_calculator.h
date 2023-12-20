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

#ifndef EMPTYLABEL_CALCULATOR_H_
#define EMPTYLABEL_CALCULATOR_H_

#include <models/results.h>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/status.h"
#include "utils/data_structures.h"
#include "utils/emptylabel.pb.h"

namespace mediapipe {

// Adds empty label to detection prediction if appropriate.
//
// Input:
//  PREDICTION - ResultObject
//
// Output:
//  PREDICTION - ResultObject
//

template <class T>
class EmptyLabelCalculator : public CalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status Open(CalculatorContext *cc) override;
  absl::Status Process(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

  T add_global_labels(const T &prediction,
                      const mediapipe::EmptyLabelOptions &options);
  Label get_label_from_options(const mediapipe::EmptyLabelOptions &options);
};

using EmptyLabelDetectionCalculator = EmptyLabelCalculator<GetiDetectionResult>;
using EmptyLabelClassificationCalculator =
    EmptyLabelCalculator<GetiClassificationResult>;
using EmptyLabelRotatedDetectionCalculator =
    EmptyLabelCalculator<RotatedDetectionResult>;
using EmptyLabelSegmentationCalculator =
    EmptyLabelCalculator<SegmentationResult>;

}  // namespace mediapipe

#endif  // EMPTYLABEL_CALCULATOR_H_
