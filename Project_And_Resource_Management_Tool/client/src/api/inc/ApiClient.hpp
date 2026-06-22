#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct ApiResponse {
    bool             success{false};
    int              httpCode{0};
    nlohmann::json   body;
    std::string      errorMessage;
};

class ApiClient {
public:
    explicit ApiClient(const std::string& baseUrl);

    ApiResponse get(const std::string& path, const std::string& token = "") const;
    ApiResponse post(const std::string& path, const nlohmann::json& payload, const std::string& token = "") const;
    ApiResponse put(const std::string& path, const nlohmann::json& payload, const std::string& token = "") const;
    ApiResponse del(const std::string& path, const std::string& token = "") const;

private:
    std::string baseUrl_;

    ApiResponse perform(
        const std::string& method,
        const std::string& path,
        const std::string& bodyStr,
        const std::string& token
    ) const;
};
