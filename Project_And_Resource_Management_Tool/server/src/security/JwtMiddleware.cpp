#include "JwtMiddleware.hpp"
#include "JwtUtil.hpp"
#include "TokenBlacklist.hpp"
#include "../utils/ResponseBuilder.hpp"
#include "../utils/AppException.hpp"

#include <string>

static constexpr auto AUTHORIZATION_HEADER = "Authorization";
static constexpr auto BEARER_PREFIX        = "Bearer ";
static constexpr size_t BEARER_PREFIX_LEN  = 7;
static constexpr auto CLAIMS_ATTRIBUTE_KEY = "tokenClaims";

void JwtMiddleware::doFilter(
    const drogon::HttpRequestPtr& request,
    drogon::FilterCallback&&      failCallback,
    drogon::FilterChainCallback&& nextCallback
) {
    const std::string authHeader = request->getHeader(AUTHORIZATION_HEADER);

    if (authHeader.empty() || authHeader.rfind(BEARER_PREFIX, 0) != 0) {
        failCallback(ResponseBuilder::error(
            "Missing or malformed Authorization header.",
            drogon::k401Unauthorized
        ));
        return;
    }

    const std::string token = authHeader.substr(BEARER_PREFIX_LEN);

    try {
        TokenClaims claims = JwtUtil::verify(token);

        if (TokenBlacklist::getInstance().contains(token)) {
            failCallback(ResponseBuilder::error(
                "Token has been invalidated. Please log in again.",
                drogon::k401Unauthorized
            ));
            return;
        }

        auto claimsPtr = std::make_shared<TokenClaims>(claims);
        request->getAttributes()->insert(CLAIMS_ATTRIBUTE_KEY, claimsPtr);

        nextCallback();

    } catch (const UnauthorizedException& authException) {
        failCallback(ResponseBuilder::error(authException.what(), drogon::k401Unauthorized));
    } catch (const std::exception& ex) {
        failCallback(ResponseBuilder::error("Token validation failed.", drogon::k401Unauthorized));
    }
}
