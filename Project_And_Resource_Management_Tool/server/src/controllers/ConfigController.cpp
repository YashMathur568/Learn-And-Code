#include "ConfigController.hpp"
#include "../repositories/MySQLConfigRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

ConfigController::ConfigController()
    : configService(std::make_shared<ConfigService>(
          std::make_shared<MySQLConfigRepository>()
      )) {}

void ConfigController::getAllConfig(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto configs = configService->getAllConfig();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& config : configs) {
            dataArray.push_back({
                {"configKey",   config.configKey},
                {"configValue", config.configValue}
            });
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ConfigController::updateConfig(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    const std::string& configKey
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string configValue = jsonBody.at("value").get<std::string>();

        configService->updateConfig(configKey, configValue);
        callback(ResponseBuilder::success({
            {"configKey",   configKey},
            {"configValue", configValue}
        }));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
