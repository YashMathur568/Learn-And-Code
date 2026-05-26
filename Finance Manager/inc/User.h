#pragma once

#include <string>

class User {
public:
    int userId;
    std::string username;
    std::string passwordHash;
    std::string salt;
    std::string createdAt;
};
