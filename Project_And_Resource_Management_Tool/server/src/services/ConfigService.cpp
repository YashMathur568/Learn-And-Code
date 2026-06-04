#include "ConfigService.hpp"
#include "../utils/AppException.hpp"

#include <algorithm>
#include <stdexcept>

const std::vector<std::string> ConfigService::ALLOWED_KEYS = {
    "llm_provider",
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
        if (value != "gemini" && value != "groq") {
            throw ValidationException("llm_provider must be 'gemini' or 'groq'.");
        }
    }

    configRepository->upsert(key, value);
}
