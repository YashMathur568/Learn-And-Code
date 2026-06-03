#pragma once

#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>
#include <string>

class ResponseBuilder {
public:
    static drogon::HttpResponsePtr success(const nlohmann::json& data,
                                           drogon::HttpStatusCode statusCode = drogon::k200OK);

    static drogon::HttpResponsePtr error(const std::string& message,
                                         drogon::HttpStatusCode statusCode);

private:
    ResponseBuilder() = delete;
};
