#include "ResponseBuilder.hpp"

drogon::HttpResponsePtr ResponseBuilder::success(
    const nlohmann::json& data,
    drogon::HttpStatusCode statusCode
) {
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(statusCode);
    response->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    response->setBody(data.dump());
    return response;
}

drogon::HttpResponsePtr ResponseBuilder::error(
    const std::string& message,
    drogon::HttpStatusCode statusCode
) {
    nlohmann::json errorBody = {
        {"success", false},
        {"message", message}
    };
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(statusCode);
    response->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    response->setBody(errorBody.dump());
    return response;
}
