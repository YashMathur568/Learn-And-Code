#pragma once

#include <string>

class Session {
public:
    int sessionId;
    int userId;
    std::string token;
    std::string expiresAt;
};
