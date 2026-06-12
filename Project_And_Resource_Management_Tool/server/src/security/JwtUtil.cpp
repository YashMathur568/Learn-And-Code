#include "JwtUtil.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ConfigLoader.hpp"

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>

#include <chrono>
#include <string>

static constexpr int TOKEN_VALIDITY_HOURS = 8;
static constexpr auto JWT_ALGORITHM       = "HS256";
static constexpr auto CLAIM_ROLE          = "role";
static constexpr auto CLAIM_USER_ID       = "userId";
static constexpr auto JWT_ISSUER          = "prm-server";

std::string JwtUtil::getSecret() {
    return ConfigLoader::getInstance().getConfig().jwtSecret;
}

std::string JwtUtil::generate(int userId, const std::string& role) {
    const auto now        = std::chrono::system_clock::now();
    const auto expiration = now + std::chrono::hours(TOKEN_VALIDITY_HOURS);

    using claim = jwt::basic_claim<jwt::traits::nlohmann_json>;

    return jwt::create<jwt::traits::nlohmann_json>()
        .set_issuer(JWT_ISSUER)
        .set_issued_at(now)
        .set_expires_at(expiration)
        .set_payload_claim(CLAIM_USER_ID, claim(std::to_string(userId)))
        .set_payload_claim(CLAIM_ROLE,    claim(role))
        .sign(jwt::algorithm::hs256{getSecret()});
}

TokenClaims JwtUtil::verify(const std::string& token) {
    try {
        auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
            .allow_algorithm(jwt::algorithm::hs256{getSecret()})
            .with_issuer(JWT_ISSUER);

        auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);
        verifier.verify(decoded);

        TokenClaims claims;
        claims.userId = std::stoi(decoded.get_payload_claim(CLAIM_USER_ID).as_string());
        claims.role   = decoded.get_payload_claim(CLAIM_ROLE).as_string();
        claims.exp    = std::chrono::system_clock::to_time_t(decoded.get_expires_at());

        return claims;

    } catch (const jwt::error::token_verification_exception& verifyEx) {
        throw UnauthorizedException(std::string("Token verification failed: ") + verifyEx.what());
    } catch (const std::exception& ex) {
        throw UnauthorizedException(std::string("Invalid token: ") + ex.what());
    }
}
