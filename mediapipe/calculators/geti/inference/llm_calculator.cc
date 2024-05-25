#include "llm_calculator.h"
#include <thread>
#include <chrono>

namespace mediapipe {
absl::Status LLMCalculator::GetContract(CalculatorContract *cc) {
  LOG(INFO) << "LLMCalculator::GetContract()";
  cc->SetProcessTimestampBounds(true);
  cc->Inputs().Tag("PROMPT").Set<std::string>().Optional();
  cc->Inputs().Tag("TICK").Set<std::string>();
  cc->Inputs().Tag("HISTORY").Set<geti::History>().Optional();

  cc->Outputs().Tag("TOKEN").Set<std::string>();
  cc->Outputs().Tag("HISTORY").Set<geti::History>().Optional();
  return absl::OkStatus();
}

absl::Status LLMCalculator::Open(CalculatorContext *cc) {
  LOG(INFO) << "LLMCalculator::Open()";
  core.add_extension("/opt/intel/openvino_2024.1.0/runtime/lib/intel64/libopenvino_tokenizers.so");

  const char* tokenizer_model_path = "/home/ronald/Projects/llm/tokenizer/openvino_tokenizer.xml";
  const char* detokenizer_model_path = "/home/ronald/Projects/llm/tokenizer/openvino_detokenizer.xml";
  const char* model_path = "/home/ronald/Projects/llm/TinyLlama-1.1B-Chat-v1.0/openvino_model.xml";

  tokenizer_model = core.read_model(tokenizer_model_path);
  detokenizer_model = core.read_model(detokenizer_model_path);
  llm_model = core.read_model(model_path);
  return absl::OkStatus();
}


absl::Status LLMCalculator::GetiProcess(CalculatorContext *cc) {
  LOG(INFO) << "LLMCalculator::GetiProcess()";
  const std::string &prompt = cc->Inputs().Tag("PROMPT").Get<std::string>();
  if (prompt.empty()) {
    // Tick from graph to output another token.
  } else {
    // Prompt was given
  }
  std::cout << "Got prompt: " << prompt << std::endl;
  Timestamp last_timestamp = Timestamp(0);//cc->InputTimestamp(); //Timestamp(0);
  geti::History history;
  history.push_back({geti::Speaker::User, prompt});
  auto [input_ids, attention_mask] = tokenize(geti::format_prompt(history));

  ov::InferRequest lm = core.compile_model(
      llm_model, "CPU").create_infer_request();

  auto seq_len = input_ids.get_size();



  lm.set_tensor("input_ids", input_ids);
  lm.set_tensor("attention_mask", attention_mask);
  ov::Tensor position_ids = lm.get_tensor("position_ids");
  position_ids.set_shape(input_ids.get_shape());
  std::iota(position_ids.data<int64_t>(), position_ids.data<int64_t>() + seq_len, 0);
  constexpr size_t BATCH_SIZE = 1;
  // Input values are persistent between inference calls.
  // That allows to set values, which aren't going to change, only once
  lm.get_tensor("beam_idx").set_shape({BATCH_SIZE});
  lm.get_tensor("beam_idx").data<int32_t>()[0] = 0;
  lm.infer();

  size_t vocab_size = lm.get_tensor("logits").get_shape().back();
  float* logits = lm.get_tensor("logits").data<float>() + (seq_len - 1) * vocab_size;
  int64_t out_token = std::max_element(logits, logits + vocab_size) - logits;

  lm.get_tensor("input_ids").set_shape({BATCH_SIZE, 1});
  position_ids.set_shape({BATCH_SIZE, 1});

  auto rt_info = tokenizer_model->get_rt_info(); //Get the runtime info for the model
  int64_t SPECIAL_EOS_TOKEN;

  if (rt_info.count("eos_token_id") > 0) { //check if the runtime information has a valid EOS token ID
      SPECIAL_EOS_TOKEN = rt_info["eos_token_id"].as<int64_t>();
  } else {
      throw std::runtime_error("EOS token ID not found in model's runtime information.");
  }

  int max_sequence_length = 1000;
  while (out_token != SPECIAL_EOS_TOKEN && seq_len < max_sequence_length) {
      //std::cout << out_token << std::endl;
      ++seq_len;
      lm.get_tensor("input_ids").data<int64_t>()[0] = out_token;
      lm.get_tensor("attention_mask").set_shape({BATCH_SIZE, seq_len});
      std::fill_n(lm.get_tensor("attention_mask").data<int64_t>(), seq_len, 1);
      position_ids.data<int64_t>()[0] = int64_t(seq_len - 1);
      lm.start_async();
      std::vector<int64_t> token = { out_token} ;
      std::string word = detokenize(token);
      //std::cout << word;
      cc->Outputs().Tag("TOKEN").AddPacket(MakePacket<std::string>(word).At(last_timestamp));
      lm.wait();
      logits = lm.get_tensor("logits").data<float>();
      out_token = std::max_element(logits, logits + vocab_size) - logits;
      ++last_timestamp;
      //std::string last_word(text.data() + print_len, text.size() - print_len);
      //std::cout << last_word << std::endl;
      //std::string output = "{ \"word\": \"" + last_word + "\", \"end\": " + (is_last_word ? "true" : "false") + "}";
  }
//end();
  lm.reset_state();
  return absl::OkStatus();
}

absl::Status LLMCalculator::Close(CalculatorContext *cc) {
  LOG(INFO) << "LLMCalculator::Close()";
  return absl::OkStatus();
}

std::pair<ov::Tensor, ov::Tensor> LLMCalculator::tokenize(std::string&& prompt) {
    constexpr size_t BATCH_SIZE = 1;
    ov::InferRequest tokenizer = core.compile_model(
        tokenizer_model, "CPU").create_infer_request();
    tokenizer.set_input_tensor(ov::Tensor{ov::element::string, {BATCH_SIZE}, &prompt});
    tokenizer.infer();
    return {tokenizer.get_tensor("input_ids"), tokenizer.get_tensor("attention_mask")};

}


std::string LLMCalculator::detokenize(std::vector<int64_t>& tokens) {
  constexpr size_t BATCH_SIZE = 1;
  ov::InferRequest detokenizer = core.compile_model(
      detokenizer_model, "CPU").create_infer_request();
  detokenizer.set_input_tensor(ov::Tensor{ov::element::i64, {BATCH_SIZE, tokens.size()}, tokens.data()});
  detokenizer.infer();
  return detokenizer.get_output_tensor().data<std::string>()[0];
}

REGISTER_CALCULATOR(LLMCalculator);
}
