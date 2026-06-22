#pragma once

#include "ILLMAdapter.hpp"
#include <string>

class GemmaAdapter : public ILLMAdapter {
public:
    GemmaAdapter(const std::string& host, const std::string& apiKey, const std::string& model);
    std::string generate(const std::string& prompt) override;

private:
    std::string host_;
    std::string apiKey_;
    std::string model_;
};
