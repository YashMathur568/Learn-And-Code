#pragma once

#include "ILLMAdapter.hpp"
#include <string>

class GeminiAdapter : public ILLMAdapter {
public:
    GeminiAdapter(const std::string& apiKey, const std::string& model);

    std::string generate(const std::string& prompt) override;

private:
    std::string apiKey_;
    std::string model_;
};
