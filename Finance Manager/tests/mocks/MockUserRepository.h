#pragma once

#include "IUserRepository.h"
#include <stdexcept>
#include <unordered_map>

class MockUserRepository : public IUserRepository {
public:
    std::unordered_map<std::string, User> users;
    bool saveCalled = false;

    void save(const User& user) override {
        saveCalled = true;
        users[user.username] = user;
    }

    User findByUsername(const std::string& username) override {
        auto it = users.find(username);
        if (it == users.end())
            throw std::runtime_error("User not found");
        return it->second;
    }

    bool existsByUsername(const std::string& username) override {
        return users.count(username) > 0;
    }
};
