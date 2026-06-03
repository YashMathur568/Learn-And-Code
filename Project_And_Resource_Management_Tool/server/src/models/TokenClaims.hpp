#pragma once

#include <string>

struct TokenClaims {
    int         userId{0};
    std::string role;
    int         employeeId{0};
    long        exp{0};
};
