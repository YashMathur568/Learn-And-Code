#pragma once

#include <string>

struct TokenClaims {
    int         userId{0};
    std::string role;
    long        exp{0};
};
