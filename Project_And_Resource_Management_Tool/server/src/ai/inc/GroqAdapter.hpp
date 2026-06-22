#pragma once

#include "ILLMAdapter.hpp"
#include <string>

class GroqAdapter : public ILLMAdapter {
public:
    GroqAdapter(const std::string& apiKey, const std::string& model);
    std::string generate(const std::string& prompt) override;

private:
    std::string apiKey_;
    std::string model_;
};
