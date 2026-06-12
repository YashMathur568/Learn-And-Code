#pragma once

#include "../models/TokenClaims.hpp"
#include <string>

class JwtUtil {
public:
    static std::string   generate(int userId, const std::string& role);
    static TokenClaims   verify(const std::string& token);

private:
    JwtUtil() = delete;

    static std::string getSecret();
};
