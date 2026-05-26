#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdint>

inline std::string generateRandomHexString(int length) {
    static const char hexCharacters[] = "0123456789abcdef";
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> distribution(0, 15);
    std::string result;
    result.reserve(length);
    for (int index = 0; index < length; ++index)
        result += hexCharacters[distribution(generator)];
    return result;
}

inline std::string generateSalt() {
    return generateRandomHexString(32);
}

inline std::string generateSessionToken() {
    return generateRandomHexString(32);
}

inline std::string hashPassword(const std::string& password, const std::string& salt) {
    std::string combined = password + salt;
    uint64_t hashValue = 14695981039346656037ULL;
    for (unsigned char byte : combined) {
        hashValue ^= byte;
        hashValue *= 1099511628211ULL;
    }
    std::ostringstream stream;
    stream << std::hex << std::setw(16) << std::setfill('0') << hashValue;
    return stream.str();
}

inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeValue = std::chrono::system_clock::to_time_t(now);
    std::ostringstream stream;
    stream << std::put_time(std::gmtime(&timeValue), "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

inline std::string getExpiryTimestamp(int hoursFromNow) {
    auto future = std::chrono::system_clock::now() + std::chrono::hours(hoursFromNow);
    std::time_t timeValue = std::chrono::system_clock::to_time_t(future);
    std::ostringstream stream;
    stream << std::put_time(std::gmtime(&timeValue), "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

inline bool isSessionExpired(const std::string& expiresAt) {
    return getCurrentTimestamp() > expiresAt;
}
