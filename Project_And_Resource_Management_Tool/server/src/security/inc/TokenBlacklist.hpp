#pragma once

#include <ctime>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class TokenBlacklist {
public:
    static TokenBlacklist& getInstance();

    void add(const std::string& token, long expiry);
    bool contains(const std::string& token);

private:
    TokenBlacklist() = default;

    void purgeExpired();

    mutable std::shared_mutex             blacklistMutex;
    std::unordered_map<std::string, long> blacklistedTokensMap;
};
