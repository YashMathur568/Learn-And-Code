#include "LLMProviderManager.hpp"
#include "GemmaAdapter.hpp"
#include "GeminiAdapter.hpp"
#include "GroqAdapter.hpp"
#include <drogon/HttpAppFramework.h>
#include <algorithm>


std::shared_ptr<LLMProviderManager> LLMProviderManager::instance_ = nullptr;
std::mutex LLMProviderManager::instanceMutex_;

LLMProviderManager::LLMProviderManager(
    std::shared_ptr<ISystemConfigRepository> configRepo,
    const std::string& initialProvider,
    const std::string& initialApiKey,
    const std::string& initialModel,
    const std::string& initialGemmaHost
) : configRepo_(configRepo) {
    currentAdapter_ = createAdapter(initialProvider, initialApiKey, initialModel, initialGemmaHost);

    if (!currentAdapter_) {
        LOG_ERROR << "Failed to initialize LLM adapter with provider: " << initialProvider;
        currentAdapter_ = std::make_shared<GemmaAdapter>(initialGemmaHost, initialApiKey, initialModel);
    }

    LOG_INFO << "LLMProviderManager initialized with provider: " << initialProvider;
}

void LLMProviderManager::setInstance(std::shared_ptr<LLMProviderManager> instance) {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    instance_ = instance;
}

std::shared_ptr<LLMProviderManager> LLMProviderManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    return instance_;
}

std::shared_ptr<ILLMAdapter> LLMProviderManager::createAdapter(
    const std::string& provider,
    const std::string& apiKey,
    const std::string& model,
    const std::string& gemmaHost
) {
    std::string lowerProvider = provider;
    std::transform(lowerProvider.begin(), lowerProvider.end(), lowerProvider.begin(), ::tolower);

    if (lowerProvider == "gemma") {
        LOG_INFO << "Creating GemmaAdapter with host: " << gemmaHost << ", model: " << model;
        return std::make_shared<GemmaAdapter>(gemmaHost, apiKey, model);
    } else if (lowerProvider == "gemini") {
        LOG_INFO << "Creating GeminiAdapter with model: " << model;
        return std::make_shared<GeminiAdapter>(apiKey, model);
    } else if (lowerProvider == "groq") {
        LOG_INFO << "Creating GroqAdapter with model: " << model;
        return std::make_shared<GroqAdapter>(apiKey, model);
    } else {
        LOG_WARN << "Unknown provider: " << provider << ", falling back to Gemma";
        return std::make_shared<GemmaAdapter>(gemmaHost, apiKey, model);
    }
}

std::shared_ptr<ILLMAdapter> LLMProviderManager::getCurrentAdapter() {
    std::lock_guard<std::mutex> lock(adapterMutex_);
    return currentAdapter_;
}

bool LLMProviderManager::reloadFromDatabase() {
    if (!configRepo_) {
        LOG_ERROR << "ConfigRepository is null, cannot reload";
        return false;
    }

    try {
        auto config = configRepo_->getAllConfig();

        std::string provider = config["llm_provider"];
        std::string apiKey = config["llm_api_key"];
        std::string model = config["llm_model"];
        std::string gemmaHost = config["gemma_llm_host"];

        if (provider.empty()) {
            LOG_WARN << "LLM provider not set in database, keeping current configuration";
            return false;
        }

        return setProvider(provider, apiKey, model, gemmaHost);
    } catch (const std::exception& dbException) {
        LOG_ERROR << "Failed to reload configuration from database: " << dbException.what();
        return false;
    }
}

bool LLMProviderManager::setProvider(
    const std::string& provider,
    const std::string& apiKey,
    const std::string& model,
    const std::string& gemmaHost
) {
    try {
        auto newAdapter = createAdapter(provider, apiKey, model, gemmaHost);

        if (!newAdapter) {
            LOG_ERROR << "Failed to create adapter for provider: " << provider;
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(adapterMutex_);
            currentAdapter_ = newAdapter;
        }

        LOG_INFO << "Successfully switched to provider: " << provider;
        return true;
    } catch (const std::exception& setException) {
        LOG_ERROR << "Failed to set provider '" << provider << "': " << setException.what();
        return false;
    }
}
