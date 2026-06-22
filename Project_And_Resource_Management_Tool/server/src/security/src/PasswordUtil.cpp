#include "PasswordUtil.hpp"
#include "AppException.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


static constexpr int PBKDF2_ITERATIONS = 100000;
static constexpr int PBKDF2_SALT_BYTES = 16;
static constexpr int PBKDF2_HASH_BYTES = 32;

static std::string toHex(const unsigned char* data, int len) {
    std::ostringstream outputStringStream;
    outputStringStream << std::hex << std::setfill('0');
    for (int byteIndex = 0; byteIndex < len; byteIndex++) {
        outputStringStream << std::setw(2) << static_cast<int>(data[byteIndex]);
    }
    return outputStringStream.str();
}

static bool fromHex(const std::string& hex, unsigned char* out, int expectedLen) {
    if (static_cast<int>(hex.size()) != expectedLen * 2) return false;
    for (int byteIndex = 0; byteIndex < expectedLen; byteIndex++) {
        try {
            out[byteIndex] = static_cast<unsigned char>(std::stoi(hex.substr(byteIndex * 2, 2), nullptr, 16));
        } catch (...) {
            return false;
        }
    }
    return true;
}

static std::vector<std::string> splitBy(const std::string& str, char delim) {
    std::vector<std::string> parts;
    std::stringstream stringStream(str);
    std::string part;
    while (std::getline(stringStream, part, delim)) {
        parts.push_back(part);
    }
    return parts;
}

std::string PasswordUtil::hash(const std::string& plainPassword) {
    unsigned char salt[PBKDF2_SALT_BYTES];
    if (RAND_bytes(salt, PBKDF2_SALT_BYTES) != 1) {
        throw AppException("Failed to generate random salt for password hashing.");
    }

    unsigned char hashOut[PBKDF2_HASH_BYTES];
    PKCS5_PBKDF2_HMAC(
        plainPassword.c_str(), static_cast<int>(plainPassword.size()),
        salt, PBKDF2_SALT_BYTES,
        PBKDF2_ITERATIONS,
        EVP_sha256(),
        PBKDF2_HASH_BYTES, hashOut
    );

    return std::string("pbkdf2:sha256:")
        + std::to_string(PBKDF2_ITERATIONS) + ":"
        + toHex(salt, PBKDF2_SALT_BYTES) + ":"
        + toHex(hashOut, PBKDF2_HASH_BYTES);
}

bool PasswordUtil::verify(const std::string& plainPassword, const std::string& storedHash) {
    const auto parts = splitBy(storedHash, ':');
    if (parts.size() != 5 || parts[0] != "pbkdf2" || parts[1] != "sha256") {
        return false;
    }

    int iterations = 0;
    try {
        iterations = std::stoi(parts[2]);
    } catch (...) {
        return false;
    }

    unsigned char salt[PBKDF2_SALT_BYTES];
    unsigned char storedBytes[PBKDF2_HASH_BYTES];
    if (!fromHex(parts[3], salt, PBKDF2_SALT_BYTES))     return false;
    if (!fromHex(parts[4], storedBytes, PBKDF2_HASH_BYTES)) return false;

    unsigned char computed[PBKDF2_HASH_BYTES];
    PKCS5_PBKDF2_HMAC(
        plainPassword.c_str(), static_cast<int>(plainPassword.size()),
        salt, PBKDF2_SALT_BYTES,
        iterations,
        EVP_sha256(),
        PBKDF2_HASH_BYTES, computed
    );


    return CRYPTO_memcmp(computed, storedBytes, PBKDF2_HASH_BYTES) == 0;
}

bool PasswordUtil::meetsStrengthPolicy(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }

    bool hasUpper   = false;
    bool hasDigit   = false;
    bool hasSpecial = false;

    for (unsigned char characterValue : password) {
        if (std::isupper(characterValue)) {
            hasUpper = true;
        } else if (std::isdigit(characterValue)) {
            hasDigit = true;
        } else if (!std::isalpha(characterValue) && !std::isdigit(characterValue)) {
            hasSpecial = true;
        }
    }

    return hasUpper && hasDigit && hasSpecial;
}
