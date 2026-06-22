#include "TokenBlacklist.hpp"

#include <ctime>
#include <mutex>

TokenBlacklist& TokenBlacklist::getInstance() {
    static TokenBlacklist instance;
    return instance;
}

void TokenBlacklist::add(const std::string& token, long expiry) {
    std::unique_lock lock(blacklistMutex);
    blacklistedTokensMap[token] = expiry;
    purgeExpired();
}

bool TokenBlacklist::contains(const std::string& token) {
    std::shared_lock lock(blacklistMutex);
    const auto iterator = blacklistedTokensMap.find(token);
    if (iterator == blacklistedTokensMap.end()) {
        return false;
    }
    if (std::time(nullptr) > iterator->second) {
        return false;
    }
    return true;
}

void TokenBlacklist::purgeExpired() {
    const long now = static_cast<long>(std::time(nullptr));
    for (auto iterator = blacklistedTokensMap.begin(); iterator != blacklistedTokensMap.end(); ) {
        if (now > iterator->second) {
            iterator = blacklistedTokensMap.erase(iterator);
        } else {
            ++iterator;
        }
    }
}
