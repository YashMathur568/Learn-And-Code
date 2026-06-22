#pragma once

#include "IConfigService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class ConfigController : public drogon::HttpController<ConfigController> {
public:
    ConfigController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ConfigController::getAllConfig,   "/api/admin/config",      drogon::Get, "JwtMiddleware");
        ADD_METHOD_TO(ConfigController::updateConfig,  "/api/admin/config/{key}", drogon::Put, "JwtMiddleware");
    METHOD_LIST_END

    void getAllConfig(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void updateConfig(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        const std::string& key
    );

private:
    std::shared_ptr<IConfigService> configService;
};
