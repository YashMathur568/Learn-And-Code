#include <drogon/drogon.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdlib>

#include "ConfigLoader.hpp"
#include "DatabasePool.hpp"
#include "AppException.hpp"
#include "AuthController.hpp"
#include "UserController.hpp"
#include "ResourceController.hpp"
#include "ProjectController.hpp"
#include "ConfigController.hpp"
#include "ManagerController.hpp"
#include "ResourceApiController.hpp"
#include "AIController.hpp"
#include "SchedulerService.hpp"
#include "NotificationService.hpp"
#include "EmailService.hpp"
#include "LLMProviderManager.hpp"
#include "MySQLSystemConfigRepository.hpp"
#include "MySQLResourceRepository.hpp"
#include "MySQLAllocationRepository.hpp"
#include "MySQLProjectRepository.hpp"
#include "MySQLMilestoneRepository.hpp"
#include "MySQLTimesheetRepository.hpp"
#include "JwtMiddleware.hpp"

int main() {
    try {
        ConfigLoader::getInstance().load("config.json");
        const AppConfig& appConfig = ConfigLoader::getInstance().getConfig();

        DatabasePool::getInstance().initialize(
            appConfig.database.host,
            appConfig.database.port,
            appConfig.database.name,
            appConfig.database.username,
            appConfig.database.password
        );


        auto resourceRepo    = std::make_shared<MySQLResourceRepository>();
        auto allocationRepo  = std::make_shared<MySQLAllocationRepository>();
        auto projectRepo     = std::make_shared<MySQLProjectRepository>();
        auto milestoneRepo   = std::make_shared<MySQLMilestoneRepository>();
        auto timesheetRepo   = std::make_shared<MySQLTimesheetRepository>();
        auto systemConfigRepo = std::make_shared<MySQLSystemConfigRepository>();


        std::string llmProvider = appConfig.llm.activeProvider;
        std::string llmApiKey;
        std::string llmModel;
        std::string gemmaHost = appConfig.llm.gemmaHost;

        if (llmProvider == "gemini") {
            llmApiKey = appConfig.llm.geminiApiKey;
            llmModel  = appConfig.llm.geminiModel;
        } else if (llmProvider == "groq") {
            llmApiKey = appConfig.llm.groqApiKey;
            llmModel  = appConfig.llm.groqModel;
        } else {
            llmApiKey = appConfig.llm.gemmaApiKey;
            llmModel  = appConfig.llm.gemmaModel;
        }

        try {
            auto dbConfig = systemConfigRepo->getAllConfig();
            if (!dbConfig["llm_provider"].empty()) {
                llmProvider = dbConfig["llm_provider"];
            }
            if (!dbConfig["llm_api_key"].empty()) {
                llmApiKey = dbConfig["llm_api_key"];
            }
            if (!dbConfig["llm_model"].empty()) {
                llmModel = dbConfig["llm_model"];
            }
            if (!dbConfig["gemma_llm_host"].empty()) {
                gemmaHost = dbConfig["gemma_llm_host"];
            }
            std::cout << "[INFO] Loaded LLM configuration from database: provider="
                      << llmProvider << std::endl;
        } catch (...) {
            std::cout << "[INFO] Using LLM configuration from config.json: provider="
                      << llmProvider << std::endl;
        }


        auto llmProviderManager = std::make_shared<LLMProviderManager>(
            systemConfigRepo,
            llmProvider,
            llmApiKey,
            llmModel,
            gemmaHost
        );
        LLMProviderManager::setInstance(llmProviderManager);

        auto emailSvc  = std::make_shared<EmailService>(appConfig.email);
        auto notifSvc  = std::make_shared<NotificationService>(
            emailSvc,
            resourceRepo,
            timesheetRepo,
            projectRepo,
            milestoneRepo,
            allocationRepo,
            llmProviderManager
        );

        SchedulerService scheduler(
            resourceRepo,
            allocationRepo,
            projectRepo,
            milestoneRepo,
            timesheetRepo,
            notifSvc,
            appConfig.schedulerIntervalHours
        );
        scheduler.start();

        const size_t threadCount = std::max(
            1u,
            std::thread::hardware_concurrency()
        );

        auto& app = drogon::app();

        app
            .setLogPath("./logs")
            .setLogLevel(trantor::Logger::kInfo)
            .addListener("0.0.0.0", appConfig.serverPort)
            .setThreadNum(static_cast<int>(threadCount));

        std::cout << "[INFO] PRM server started on http://localhost:"
                  << appConfig.serverPort
                  << " with " << threadCount << " worker thread(s)."
                  << std::endl;

        app.run();

    } catch (const AppException& appException) {
        std::cerr << "[FATAL] " << appException.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& stdException) {
        std::cerr << "[FATAL] " << stdException.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
