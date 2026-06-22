#include "ConfigService.hpp"
#include "AppException.hpp"
#include "LLMProviderManager.hpp"

#include <algorithm>
#include <stdexcept>

const std::vector<std::string> ConfigService::ALLOWED_KEYS = {
    "llm_provider",
    "llm_api_key",
    "llm_model",
    "gemma_llm_host",
    "scheduler_interval_hours",
    "max_weekly_hours"
};

ConfigService::ConfigService(std::shared_ptr<IConfigRepository> configRepository)
    : configRepository(std::move(configRepository)) {}

void ConfigService::validateKnownKey(const std::string& key) const {
    bool keyAllowed = std::any_of(
        ALLOWED_KEYS.begin(), ALLOWED_KEYS.end(),
        [&key](const std::string& allowedKey) { return allowedKey == key; }
    );
    if (!keyAllowed) {
        throw ValidationException("Config key '" + key + "' is not recognized or not editable.");
    }
}

std::vector<SystemConfig> ConfigService::getAllConfig() {
    return configRepository->findAll();
}

void ConfigService::updateConfig(const std::string& key, const std::string& value) {
    validateKnownKey(key);

    if (value.empty()) {
        throw ValidationException("Config value cannot be empty.");
    }

    if (key == "scheduler_interval_hours" || key == "max_weekly_hours") {
        try {
            int numericValue = std::stoi(value);
            if (numericValue < 1) {
                throw ValidationException("'" + key + "' must be a positive integer.");
            }
        } catch (const std::invalid_argument&) {
            throw ValidationException("'" + key + "' must be a valid integer.");
        } catch (const std::out_of_range&) {
            throw ValidationException("'" + key + "' value is out of range.");
        }
    }

    if (key == "llm_provider") {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        if (lowerValue != "gemini" && lowerValue != "groq" && lowerValue != "gemma") {
            throw ValidationException("llm_provider must be 'gemini', 'groq', or 'gemma'.");
        }
    }

    configRepository->upsert(key, value);


    auto llmProviderManager = LLMProviderManager::getInstance();
    if (llmProviderManager &&
        (key == "llm_provider" || key == "llm_api_key" ||
         key == "llm_model" || key == "gemma_llm_host")) {
        llmProviderManager->reloadFromDatabase();
    }
}
