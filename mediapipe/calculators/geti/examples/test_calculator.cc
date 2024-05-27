#include "test_calculator.h"

#include <chrono>
#include <thread>

namespace mediapipe {

absl::Status TestCalculator::GetContract(CalculatorContract *cc) {
  cc->Inputs().Tag("PROMPT").Set<std::string>();
  cc->Outputs().Tag("TOKEN").Set<std::string>();
  //cc->SetProcessTimestampBounds(true);
  cc->SetTimestampOffset(5);
  return absl::OkStatus();
}

absl::Status TestCalculator::Open(CalculatorContext *cc) {
  return absl::OkStatus();
}

absl::Status TestCalculator::Process(CalculatorContext *cc) {
  for (size_t i = 0; i < 10; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string word = "Hello world";
    auto packet = MakePacket<std::string>(word);
    std::cout << "puts" << std::endl;
    cc->Outputs().Tag("TOKEN").AddPacket(packet.At(cc->InputTimestamp() + i));
    cc->Outputs().Tag("TOKEN").SetNextTimestampBound(cc->InputTimestamp() + i);
  }

  return absl::OkStatus();
}

absl::Status TestCalculator::Close(CalculatorContext *cc) {
  return absl::OkStatus();
}

REGISTER_CALCULATOR(TestCalculator);

} // namespace mediapipe
