#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include "AuthService.hpp"
#include "AppException.hpp"
#include "ConfigLoader.hpp"
#include "PasswordUtil.hpp"
#include "mocks/MockUserRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;


static void bootstrapConfigForAuth() {
    static bool done = false;
    if (done) return;
    done = true;

    const char* minimalConfig = R"({
        "database": {"host":"localhost","port":3306,"name":"prm","username":"root","password":""},
        "llm":      {"provider":"gemma","apiKey":"","geminiModel":"","groqModel":"","gemmaHost":"","gemmaModel":""},
        "email":    {"enabled":false,"smtpHost":"","smtpPort":587,"username":"","password":"","fromAddress":""},
        "jwtSecret":              "test_secret",
        "serverPort":             8080,
        "schedulerIntervalHours": 4,
        "maxWeeklyHours":         40
    })";

    std::ofstream out("test_config.json");
    if (out.is_open()) {
        out << minimalConfig;
        out.close();
        try { ConfigLoader::getInstance().load("test_config.json"); } catch (...) {}
    }
}

static User makeActiveUser(int id = 1, const std::string& passwordHash = "pbkdf2:sha256:1:aa:bb") {
    User user;
    user.userId           = id;
    user.username         = "testuser";
    user.passwordHash     = passwordHash;
    user.role             = "RESOURCE";
    user.isActive         = true;
    user.passwordExpiresAt = "";
    return user;
}

class AuthServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockUserRepository> userRepo;
    std::unique_ptr<AuthService>        service;

    void SetUp() override {
        bootstrapConfigForAuth();
        userRepo = std::make_shared<NiceMock<MockUserRepository>>();
        service  = std::make_unique<AuthService>(userRepo);
    }
};


// ── login ─────────────────────────────────────────────────────────────────────

TEST_F(AuthServiceTest, Login_UserNotFound_ThrowsUnauthorized) {
    EXPECT_CALL(*userRepo, findByUsername("nobody")).WillOnce(Return(std::nullopt));
    LoginRequest req{"nobody", "pass"};
    EXPECT_THROW(service->login(req), UnauthorizedException);
}

TEST_F(AuthServiceTest, Login_InactiveUser_ThrowsUnauthorized) {
    User user = makeActiveUser();
    user.isActive = false;
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    EXPECT_THROW(service->login({"testuser", "pass"}), UnauthorizedException);
}

TEST_F(AuthServiceTest, Login_WrongPassword_ThrowsUnauthorized) {
    User user = makeActiveUser();
    user.passwordHash     = PasswordUtil::hash("CorrectPass1!");
    user.passwordExpiresAt = "2099-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    EXPECT_THROW(service->login({"testuser", "WrongPass1!"}), UnauthorizedException);
}

TEST_F(AuthServiceTest, Login_CorrectPassword_ReturnsNonEmptyToken) {
    User user = makeActiveUser();
    user.passwordHash     = PasswordUtil::hash("Valid1!");
    user.passwordExpiresAt = "2099-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    LoginResponse resp = service->login({"testuser", "Valid1!"});
    EXPECT_FALSE(resp.token.empty());
    EXPECT_EQ(resp.role, "RESOURCE");
}

TEST_F(AuthServiceTest, Login_EmptyPasswordExpiry_ForceChangeTrue) {
    User user = makeActiveUser();
    user.passwordHash     = PasswordUtil::hash("Valid1!");
    user.passwordExpiresAt = "";
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    LoginResponse resp = service->login({"testuser", "Valid1!"});
    EXPECT_TRUE(resp.forcePasswordChange);
}

TEST_F(AuthServiceTest, Login_ExpiredPassword_ForceChangeTrue) {
    User user = makeActiveUser();
    user.passwordHash     = PasswordUtil::hash("Valid1!");
    user.passwordExpiresAt = "2000-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    LoginResponse resp = service->login({"testuser", "Valid1!"});
    EXPECT_TRUE(resp.forcePasswordChange);
}

TEST_F(AuthServiceTest, Login_FutureExpiry_ForceChangeFalse) {
    User user = makeActiveUser();
    user.passwordHash     = PasswordUtil::hash("Valid1!");
    user.passwordExpiresAt = "2099-12-31 00:00:00";
    EXPECT_CALL(*userRepo, findByUsername(_)).WillOnce(Return(user));
    LoginResponse resp = service->login({"testuser", "Valid1!"});
    EXPECT_FALSE(resp.forcePasswordChange);
}


// ── changePassword ────────────────────────────────────────────────────────────

TEST_F(AuthServiceTest, ChangePassword_MismatchPasswords_ThrowsValidation) {
    ChangePasswordRequest req{"Old1!", "NewPass1!", "NewPass1?Diff"};
    EXPECT_THROW(service->changePassword(1, req), ValidationException);
}

TEST_F(AuthServiceTest, ChangePassword_WeakNewPassword_ThrowsValidation) {
    ChangePasswordRequest req{"Old1!", "weak", "weak"};
    EXPECT_THROW(service->changePassword(1, req), ValidationException);
}

TEST_F(AuthServiceTest, ChangePassword_UserNotFound_ThrowsNotFound) {
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    ChangePasswordRequest req{"Old1!", "NewPass1!", "NewPass1!"};
    EXPECT_THROW(service->changePassword(99, req), NotFoundException);
}

TEST_F(AuthServiceTest, ChangePassword_ExpiredPassword_SkipsCurrentCheck) {
    User user = makeActiveUser();
    user.passwordExpiresAt = "2000-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_CALL(*userRepo, updatePasswordHash(1, _)).Times(1);
    EXPECT_CALL(*userRepo, refreshPasswordExpiry(1)).Times(1);

    ChangePasswordRequest req{"any-old-pass", "NewPass1!", "NewPass1!"};
    EXPECT_NO_THROW(service->changePassword(1, req));
}

TEST_F(AuthServiceTest, ChangePassword_NotExpired_WrongCurrent_ThrowsUnauthorized) {
    User user = makeActiveUser();
    user.passwordHash      = PasswordUtil::hash("CorrectOld1!");
    user.passwordExpiresAt = "2099-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));

    ChangePasswordRequest req{"WrongOld1!", "NewPass1!", "NewPass1!"};
    EXPECT_THROW(service->changePassword(1, req), UnauthorizedException);
}

TEST_F(AuthServiceTest, ChangePassword_NotExpired_CorrectCurrent_Success) {
    User user = makeActiveUser();
    user.passwordHash      = PasswordUtil::hash("CorrectOld1!");
    user.passwordExpiresAt = "2099-01-01 00:00:00";
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_CALL(*userRepo, updatePasswordHash(1, _)).Times(1);
    EXPECT_CALL(*userRepo, refreshPasswordExpiry(1)).Times(1);

    ChangePasswordRequest req{"CorrectOld1!", "NewPass1!", "NewPass1!"};
    EXPECT_NO_THROW(service->changePassword(1, req));
}
