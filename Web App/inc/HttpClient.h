#pragma once
#include "../interfaces/IHttpClient.h"
#include <string>

class HttpClient : public IHttpClient {
public:
    HttpClient();
    ~HttpClient() override;

    std::string sendGetRequest(const std::string& url) override;
    std::string urlEncode(const std::string& value) override;
};
