#include <gtest/gtest.h>
#include "AuthUtils.h"
#include <thread>
#include <chrono>

// ─── hashPassword ─────────────────────────────────────────────────────────────

TEST(AuthUtils, SameInputProducesSameHash) {
    std::string hash1 = hashPassword("mypassword", "mysalt");
    std::string hash2 = hashPassword("mypassword", "mysalt");
    EXPECT_EQ(hash1, hash2);
}

TEST(AuthUtils, DifferentSaltProducesDifferentHash) {
    std::string hash1 = hashPassword("mypassword", "salt_aaa");
    std::string hash2 = hashPassword("mypassword", "salt_bbb");
    EXPECT_NE(hash1, hash2);
}

TEST(AuthUtils, DifferentPasswordProducesDifferentHash) {
    std::string hash1 = hashPassword("password1", "samesalt");
    std::string hash2 = hashPassword("password2", "samesalt");
    EXPECT_NE(hash1, hash2);
}

TEST(AuthUtils, HashIsNotPlaintext) {
    std::string hash = hashPassword("secret", "somesalt");
    EXPECT_NE(hash, "secret");
    EXPECT_NE(hash, "somesalt");
}

TEST(AuthUtils, HashIsHexString) {
    std::string hash = hashPassword("pass", "salt");
    EXPECT_EQ(hash.size(), 16u);
    for (char c : hash)
        EXPECT_TRUE(std::isxdigit(static_cast<unsigned char>(c)));
}

// ─── generateSalt ─────────────────────────────────────────────────────────────

TEST(AuthUtils, GeneratedSaltIsNonEmpty) {
    EXPECT_FALSE(generateSalt().empty());
}

TEST(AuthUtils, GeneratedSaltsAreUnique) {
    EXPECT_NE(generateSalt(), generateSalt());
}

TEST(AuthUtils, GeneratedSaltIsHex) {
    std::string salt = generateSalt();
    for (char c : salt)
        EXPECT_TRUE(std::isxdigit(static_cast<unsigned char>(c)));
}

// ─── generateSessionToken ─────────────────────────────────────────────────────

TEST(AuthUtils, GeneratedTokensAreUnique) {
    EXPECT_NE(generateSessionToken(), generateSessionToken());
}

TEST(AuthUtils, GeneratedTokenIsHex) {
    std::string token = generateSessionToken();
    for (char c : token)
        EXPECT_TRUE(std::isxdigit(static_cast<unsigned char>(c)));
}

// ─── isSessionExpired ─────────────────────────────────────────────────────────

TEST(AuthUtils, FutureTimestampIsNotExpired) {
    std::string future = getExpiryTimestamp(24);
    EXPECT_FALSE(isSessionExpired(future));
}

TEST(AuthUtils, PastTimestampIsExpired) {
    EXPECT_TRUE(isSessionExpired("2000-01-01T00:00:00Z"));
}

TEST(AuthUtils, NearFutureIsNotExpired) {
    std::string nearFuture = getExpiryTimestamp(1);
    EXPECT_FALSE(isSessionExpired(nearFuture));
}

// ─── getExpiryTimestamp ───────────────────────────────────────────────────────

TEST(AuthUtils, ExpiryTimestampIsInFuture) {
    std::string now    = getCurrentTimestamp();
    std::string expiry = getExpiryTimestamp(1);
    EXPECT_GT(expiry, now);
}

TEST(AuthUtils, LongerExpiryIsLater) {
    std::string expiry1h  = getExpiryTimestamp(1);
    std::string expiry24h = getExpiryTimestamp(24);
    EXPECT_LT(expiry1h, expiry24h);
}
