#include "TokenBlacklist.hpp"

#include <ctime>
#include <mutex>

TokenBlacklist& TokenBlacklist::getInstance() {
    static TokenBlacklist instance;
    return instance;
}

void TokenBlacklist::add(const std::string& token, long expiry) {
    std::unique_lock lock(mutex_);
    blacklist_[token] = expiry;
    purgeExpired();
}

bool TokenBlacklist::contains(const std::string& token) {
    std::shared_lock lock(mutex_);
    const auto iterator = blacklist_.find(token);
    if (iterator == blacklist_.end()) {
        return false;
    }
    if (std::time(nullptr) > iterator->second) {
        return false;
    }
    return true;
}

void TokenBlacklist::purgeExpired() {
    const long now = static_cast<long>(std::time(nullptr));
    for (auto iterator = blacklist_.begin(); iterator != blacklist_.end(); ) {
        if (now > iterator->second) {
            iterator = blacklist_.erase(iterator);
        } else {
            ++iterator;
        }
    }
}
