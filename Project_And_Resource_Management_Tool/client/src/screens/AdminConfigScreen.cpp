#include "AdminConfigScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

static void printConfigRow(const std::string& label, const std::string& value, bool mask = false) {
    const std::string display = mask ? std::string(value.size(), '*') : value;
    std::cout << "  " << ConsoleUtil::col(label, 24) << ":  " << display << "\n";
}

static std::string updateKey(const ApiClient& api, const std::string& key, const std::string& newValue) {
    const auto resp = api.put(
        "/api/admin/config/" + key,
        {{"value", newValue}},
        AppSession::get().token
    );
    return resp.success ? "" : resp.errorMessage;
}

void showAdminConfigMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("SYSTEM CONFIGURATION");

        const auto resp = api.get("/api/admin/config", AppSession::get().token);
        std::string llmProvider, llmApiKey, schedulerInterval, maxWeeklyHours;

        if (resp.success) {
            const auto& data = resp.body.value("data", nlohmann::json::array());
            for (const auto& cfg : data) {
                const std::string configKey   = cfg.value("configKey", "");
                const std::string configValue = cfg.value("configValue", "");
                if (configKey == "llm_provider")           llmProvider       = configValue;
                else if (configKey == "llm_api_key")        llmApiKey         = configValue;
                else if (configKey == "scheduler_interval_hours") schedulerInterval = configValue;
                else if (configKey == "max_weekly_hours")   maxWeeklyHours    = configValue;
            }
        }

        std::cout << "Current Settings:\n";
        printConfigRow("LLM Provider",        llmProvider.empty()       ? "(not set)" : llmProvider);
        printConfigRow("LLM API Key",         llmApiKey.empty()         ? "(not set)" : llmApiKey, true);
        printConfigRow("Scheduler Interval",  schedulerInterval.empty() ? "(not set)" : schedulerInterval + " hours");
        printConfigRow("Max Weekly Hours",    maxWeeklyHours.empty()    ? "(not set)" : maxWeeklyHours);

        std::cout << "\n1. Update LLM API Key\n"
                  << "2. Change LLM Provider  (gemini / groq)\n"
                  << "3. Update Scheduler Interval (hours)\n"
                  << "4. Update Max Weekly Hours\n"
                  << "5. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "5" || opt == "b" || opt == "B") return;

        std::string newValue;
        std::string key;

        if (opt == "1") {
            newValue = ConsoleUtil::promptInput("New API Key: ");
            key      = "llm_api_key";
        } else if (opt == "2") {
            std::cout << "Provider (gemini / groq): ";
            std::getline(std::cin, newValue);
            key = "llm_provider";
        } else if (opt == "3") {
            newValue = ConsoleUtil::promptInput("Interval in hours: ");
            key      = "scheduler_interval_hours";
        } else if (opt == "4") {
            newValue = ConsoleUtil::promptInput("Max weekly hours: ");
            key      = "max_weekly_hours";
        } else {
            continue;
        }

        if (newValue.empty()) continue;

        const std::string err = updateKey(api, key, newValue);
        if (!err.empty()) ConsoleUtil::printError(err);
        else              ConsoleUtil::printSuccess("Setting updated.");
        ConsoleUtil::pause();
    }
}
