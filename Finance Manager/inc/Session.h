#pragma once

#include <string>

struct Session {
    int id;
    int userId;
    std::string token;
    std::string expiresAt;
};
