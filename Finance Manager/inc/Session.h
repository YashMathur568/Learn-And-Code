#pragma once

#include <string>

class Session {
public:
    int id;
    int userId;
    std::string token;
    std::string expiresAt;
};
