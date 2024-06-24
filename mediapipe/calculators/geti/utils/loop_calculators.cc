#include "../utils/loop_calculators.h"

namespace mediapipe {

REGISTER_CALCULATOR(BeginLoopRectanglePredictionCalculator);
REGISTER_CALCULATOR(EndLoopRectanglePredictionsCalculator);
REGISTER_CALCULATOR(EndLoopPolygonPredictionsCalculator);
REGISTER_CALCULATOR(BeginLoopModelApiDetectionCalculator);
REGISTER_CALCULATOR(EndLoopModelApiDetectionClassificationCalculator);
REGISTER_CALCULATOR(EndLoopModelApiDetectionSegmentationCalculator);

}  // namespace mediapipe
