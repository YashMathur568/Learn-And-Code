#pragma once

#include "ILLMAdapter.hpp"
#include "ConfigLoader.hpp"
#include "GeminiAdapter.hpp"
#include "GroqAdapter.hpp"
#include "GemmaAdapter.hpp"

#include <memory>
#include <stdexcept>

class LLMAdapterFactory {
public:
    static std::shared_ptr<ILLMAdapter> create() {
        const auto& config = ConfigLoader::getInstance().getConfig();
        const std::string provider = config.llm.activeProvider;

        if (provider == "gemini") {
            return std::make_shared<GeminiAdapter>(config.llm.geminiApiKey, config.llm.geminiModel);
        }

        if (provider == "groq") {
            return std::make_shared<GroqAdapter>(config.llm.groqApiKey, config.llm.groqModel);
        }

        if (provider == "gemma") {
            return std::make_shared<GemmaAdapter>(
                config.llm.gemmaHost, config.llm.gemmaApiKey, config.llm.gemmaModel);
        }

        throw std::runtime_error("Unknown LLM provider: " + provider);
    }
};
