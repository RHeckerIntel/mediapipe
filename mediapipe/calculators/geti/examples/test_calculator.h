#ifndef TEST_CALCULATOR_H
#define TEST_CALCULATOR_H

#include "mediapipe/framework/calculator_framework.h"


namespace mediapipe {

class TestCalculator : public CalculatorBase {
    public:
    static absl::Status GetContract(CalculatorContract *cc);
    absl::Status Open(CalculatorContext *cc) override;
    absl::Status Process(CalculatorContext *cc) override;
    absl::Status Close(CalculatorContext *cc) override;

    Timestamp internal_timestamp = Timestamp(0);
};

}

#endif
