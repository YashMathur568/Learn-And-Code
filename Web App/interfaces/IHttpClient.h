#pragma once
#include <string>

class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual std::string sendGetRequest(const std::string& url) = 0;
    virtual std::string urlEncode(const std::string& value) = 0;
};
