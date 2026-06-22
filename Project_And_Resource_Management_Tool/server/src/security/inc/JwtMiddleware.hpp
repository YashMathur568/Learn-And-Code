#pragma once

#include <drogon/HttpFilter.h>

class JwtMiddleware : public drogon::HttpFilter<JwtMiddleware> {
public:
    void doFilter(const drogon::HttpRequestPtr& request,
                  drogon::FilterCallback&&      failCallback,
                  drogon::FilterChainCallback&& nextCallback) override;
};
