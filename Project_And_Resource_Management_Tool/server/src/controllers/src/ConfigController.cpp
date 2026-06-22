#include "ConfigController.hpp"
#include "ConfigService.hpp"
#include "MySQLConfigRepository.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"

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
        PermissionGuard::require(request, "MANAGE_CONFIG");

        const auto configs = configService->getAllConfig();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& config : configs) {
            dataArray.push_back({
                {"configKey",   config.configKey},
                {"configValue", config.configValue}
            });
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ConfigController::updateConfig(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    const std::string& configKey
) {
    try {
        PermissionGuard::require(request, "MANAGE_CONFIG");

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string configValue = jsonBody.at("value").get<std::string>();

        configService->updateConfig(configKey, configValue);
        callback(ResponseBuilder::success({
            {"configKey",   configKey},
            {"configValue", configValue}
        }));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}
