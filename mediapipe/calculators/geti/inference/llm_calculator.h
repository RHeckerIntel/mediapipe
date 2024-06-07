#ifndef LLM_CALCULATOR_H_
#define LLM_CALCULATOR_H_

#include <memory>

#include <openvino/openvino.hpp>
#include "../inference/geti_calculator_base.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/opencv_core_inc.h"
#include "mediapipe/framework/port/status.h"

namespace geti {
  enum Speaker {
      User,
      Assistant,
      System
  };

  using History = std::vector<std::pair<Speaker, std::string>>;

  inline std::string formatSpeaker(const Speaker& speaker) {
      switch(speaker){
          case System:
              return "<|system|>";
          case Assistant:
              return "<|assistant|>";
          case User:
              return "<|user|>";
      }
  }

  inline std::string format_prompt(History history) {
    std::string prompt = "";
    for (size_t i = 0; i < history.size(); i++) {
        prompt += formatSpeaker(history[i].first) + "\n" + history[i].second + "</s>\n";
    }

    return prompt + formatSpeaker(Assistant) + "\n";
  }
}

namespace mediapipe {

// Runs detection inference on the provided image and OpenVINO model.
//
// Input:
//  PROMPT - std::string
//
// Output:
//   HISTORY - std::vector<History> (on last token)
//   TOKEN - std::string (word)
//

class LLMCalculator : public GetiCalculatorBase {
 public:
  static absl::Status GetContract(CalculatorContract *cc);
  absl::Status GetiOpen(CalculatorContext *cc) override;
  absl::Status GetiProcess(CalculatorContext *cc) override;
  absl::Status Close(CalculatorContext *cc) override;

 private:
  ov::Core core;
  std::shared_ptr<ov::Model> tokenizer_model, detokenizer_model, llm_model;

  std::pair<ov::Tensor, ov::Tensor> tokenize(std::string&& prompt);
  std::string detokenize(std::vector<int64_t>& tokens);

 private: // llm state stuff
  geti::History history;
  size_t seq_len;
  size_t print_len;
  int64_t out_token;
  float* logits;
  size_t vocab_size;
  ov::InferRequest lm;
  ov::Tensor position_ids;
  std::string text;
  std::vector<int64_t> token_cache;



  int64_t SPECIAL_EOS_TOKEN;
  static constexpr size_t BATCH_SIZE = 1;
  static constexpr size_t max_sequence_length = 1000;
};
}  // namespace mediapipe



#endif // LLM_CALCULATOR_H_
