#pragma once

#include <string>

struct User {
    int         userId{0};
    std::string fullName;
    std::string email;
    std::string username;
    std::string passwordHash;
    std::string role;
    bool        isActive{true};
    bool        forcePwdChange{true};
    std::string createdAt;
};
