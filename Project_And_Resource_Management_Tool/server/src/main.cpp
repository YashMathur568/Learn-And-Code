#include <drogon/drogon.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdlib>

#include "utils/ConfigLoader.hpp"
#include "utils/DatabasePool.hpp"
#include "utils/AppException.hpp"
#include "controllers/AuthController.hpp"
#include "controllers/UserController.hpp"
#include "controllers/EmployeeController.hpp"
#include "controllers/ProjectController.hpp"
#include "controllers/ConfigController.hpp"
#include "security/JwtMiddleware.hpp"

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

        const size_t threadCount = std::max(
            1u,
            std::thread::hardware_concurrency()
        );

        drogon::app()
            .setLogPath("./logs")
            .setLogLevel(trantor::Logger::kInfo)
            .addListener("0.0.0.0", appConfig.serverPort)
            .setThreadNum(static_cast<int>(threadCount))
            .run();

    } catch (const AppException& appException) {
        std::cerr << "[FATAL] " << appException.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& stdException) {
        std::cerr << "[FATAL] " << stdException.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
