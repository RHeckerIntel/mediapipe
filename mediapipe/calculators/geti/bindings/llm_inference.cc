#include "llm_inference.h"

void LLMInference::set_streamer(bool (*callback)(const char*)) {
    streamer = [callback](std::string word) {
        callback(word.c_str());
        return false;
    };
}

std::string LLMInference::prompt(std::string message) {
    history.push_back({{"role", "user"}, {"content", message}});
    auto prompt = pipe.get_tokenizer().apply_chat_template(history, true);

    ov::genai::GenerationConfig config;
    config.max_new_tokens = 1000;
    if (streamer) {
        std::string result = pipe.generate(prompt, config, streamer);
        history.push_back({{"role", "assistant"}, {"content", result}});
        return result;
    } else {
        std::string result = pipe.generate(prompt, config);
        return result;
    }
}
