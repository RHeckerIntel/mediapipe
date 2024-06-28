#ifndef LLM_INFERENCE_H_
#define LLM_INFERENCE_H_

#include "openvino/genai/llm_pipeline.hpp"

class LLMInference {
  ov::genai::LLMPipeline pipe;
  ov::genai::ChatHistory history;
  std::function<bool(std::string)> streamer;
  public:
    LLMInference(std::string device, std::string model_path): pipe(model_path, device) {}
    void set_streamer(bool (*callback)(const char*));
    std::string prompt(std::string message);

};

#endif // LLM_INFERENCE_H_
