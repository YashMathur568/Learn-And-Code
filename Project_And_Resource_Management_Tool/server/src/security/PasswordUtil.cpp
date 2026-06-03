#include "PasswordUtil.hpp"
#include "../utils/AppException.hpp"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <crypt.h>
#include <openssl/rand.h>

#include <cctype>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

static constexpr int    BCRYPT_WORK_FACTOR      = 12;
static constexpr size_t BCRYPT_RANDOM_BYTE_COUNT = 16;

static std::string buildBcryptSalt(int workFactor) {
    unsigned char randomBytes[BCRYPT_RANDOM_BYTE_COUNT];
    if (RAND_bytes(randomBytes, static_cast<int>(BCRYPT_RANDOM_BYTE_COUNT)) != 1) {
        throw AppException("Failed to generate random bytes for bcrypt salt.");
    }

    static constexpr char BASE64_ALPHABET[] =
        "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::string encoded;
    encoded.reserve(22);
    for (size_t index = 0; index < 16 && encoded.size() < 22; index += 3) {
        unsigned int triplet = static_cast<unsigned int>(randomBytes[index]);
        if (index + 1 < BCRYPT_RANDOM_BYTE_COUNT) triplet |= static_cast<unsigned int>(randomBytes[index + 1]) << 8;
        if (index + 2 < BCRYPT_RANDOM_BYTE_COUNT) triplet |= static_cast<unsigned int>(randomBytes[index + 2]) << 16;

        encoded += BASE64_ALPHABET[triplet & 0x3f];
        if (encoded.size() < 22) encoded += BASE64_ALPHABET[(triplet >> 6)  & 0x3f];
        if (encoded.size() < 22) encoded += BASE64_ALPHABET[(triplet >> 12) & 0x3f];
        if (encoded.size() < 22) encoded += BASE64_ALPHABET[(triplet >> 18) & 0x3f];
    }
    encoded.resize(22);

    std::ostringstream saltBuilder;
    saltBuilder << "$2b$" << std::setw(2) << std::setfill('0') << workFactor << "$" << encoded;
    return saltBuilder.str();
}

std::string PasswordUtil::hash(const std::string& plainPassword) {
    const std::string salt = buildBcryptSalt(BCRYPT_WORK_FACTOR);

    struct crypt_data cryptData{};
    cryptData.initialized = 0;

    const char* result = crypt_r(plainPassword.c_str(), salt.c_str(), &cryptData);
    if (result == nullptr || std::strncmp(result, "$2b$", 4) != 0) {
        throw AppException("Password hashing failed.");
    }

    return std::string(result);
}

bool PasswordUtil::verify(const std::string& plainPassword, const std::string& storedHash) {
    struct crypt_data cryptData{};
    cryptData.initialized = 0;

    const char* result = crypt_r(plainPassword.c_str(), storedHash.c_str(), &cryptData);
    if (result == nullptr) {
        return false;
    }

    return storedHash == std::string(result);
}

bool PasswordUtil::meetsStrengthPolicy(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }

    bool hasUpper   = false;
    bool hasDigit   = false;
    bool hasSpecial = false;

    for (unsigned char ch : password) {
        if (std::isupper(ch)) {
            hasUpper = true;
        } else if (std::isdigit(ch)) {
            hasDigit = true;
        } else if (!std::isalpha(ch) && !std::isdigit(ch)) {
            hasSpecial = true;
        }
    }

    return hasUpper && hasDigit && hasSpecial;
}
