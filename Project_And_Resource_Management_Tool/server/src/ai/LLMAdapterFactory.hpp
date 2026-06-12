#pragma once

#include "ILLMAdapter.hpp"
#include "../utils/ConfigLoader.hpp"
#include "GeminiAdapter.hpp"
#include "GroqAdapter.hpp"
#include "CompanyAdapter.hpp"

#include <memory>
#include <stdexcept>

class LLMAdapterFactory {
public:
    static std::shared_ptr<ILLMAdapter> create() {
        const auto& config = ConfigLoader::getInstance().getConfig();
        const std::string provider = config.llm.provider;

        if (provider == "gemini") {
            return std::make_shared<GeminiAdapter>(config.llm.apiKey, config.llm.geminiModel);
        }

        if (provider == "groq") {
            return std::make_shared<GroqAdapter>(config.llm.apiKey, config.llm.groqModel);
        }

        if (provider == "company") {
            return std::make_shared<CompanyAdapter>(
                config.llm.companyHost, config.llm.apiKey, config.llm.companyModel);
        }

        throw std::runtime_error("Unknown LLM provider: " + provider);
    }
};
