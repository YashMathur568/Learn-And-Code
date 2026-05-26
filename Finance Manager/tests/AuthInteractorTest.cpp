#include <gtest/gtest.h>
#include "RegisterInteractor.h"
#include "LoginInteractor.h"
#include "LogoutInteractor.h"
#include "ValidateTokenInteractor.h"
#include "AuthUtils.h"
#include "../mocks/MockUserRepository.h"
#include "../mocks/MockSessionRepository.h"

// ─── RegisterInteractor ───────────────────────────────────────────────────────

TEST(RegisterInteractor, SuccessfulRegistration) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    interactor.execute("alice", "pass1234");
    EXPECT_TRUE(userRepo.saveCalled);
    EXPECT_TRUE(userRepo.existsByUsername("alice"));
}

TEST(RegisterInteractor, EmptyUsernameThrows) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    EXPECT_THROW(interactor.execute("", "pass1234"), std::runtime_error);
}

TEST(RegisterInteractor, ShortPasswordThrows) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    EXPECT_THROW(interactor.execute("alice", "abc"), std::runtime_error);
}

TEST(RegisterInteractor, PasswordExactlyFourCharsSucceeds) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    EXPECT_NO_THROW(interactor.execute("bob", "1234"));
}

TEST(RegisterInteractor, DuplicateUsernameThrows) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    interactor.execute("alice", "pass1234");
    EXPECT_THROW(interactor.execute("alice", "other123"), std::runtime_error);
}

TEST(RegisterInteractor, PasswordIsStoredHashed) {
    MockUserRepository userRepo;
    RegisterInteractor interactor(userRepo);
    interactor.execute("alice", "pass1234");
    User saved = userRepo.findByUsername("alice");
    EXPECT_NE(saved.passwordHash, "pass1234");
    EXPECT_FALSE(saved.salt.empty());
}

// ─── LoginInteractor ──────────────────────────────────────────────────────────

static void registerUser(MockUserRepository& repo, const std::string& username, const std::string& password) {
    RegisterInteractor reg(repo);
    reg.execute(username, password);
}

TEST(LoginInteractor, SuccessfulLoginReturnsToken) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    registerUser(userRepo, "alice", "pass1234");
    LoginInteractor interactor(userRepo, sessionRepo);
    std::string token = interactor.execute("alice", "pass1234");
    EXPECT_FALSE(token.empty());
    EXPECT_TRUE(sessionRepo.saveCalled);
}

TEST(LoginInteractor, WrongPasswordThrows) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    registerUser(userRepo, "alice", "pass1234");
    LoginInteractor interactor(userRepo, sessionRepo);
    EXPECT_THROW(interactor.execute("alice", "wrongpass"), std::runtime_error);
}

TEST(LoginInteractor, UnknownUsernameThrows) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    LoginInteractor interactor(userRepo, sessionRepo);
    EXPECT_THROW(interactor.execute("nobody", "pass1234"), std::runtime_error);
}

TEST(LoginInteractor, EmptyUsernameThrows) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    LoginInteractor interactor(userRepo, sessionRepo);
    EXPECT_THROW(interactor.execute("", "pass1234"), std::runtime_error);
}

TEST(LoginInteractor, EmptyPasswordThrows) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    LoginInteractor interactor(userRepo, sessionRepo);
    EXPECT_THROW(interactor.execute("alice", ""), std::runtime_error);
}

TEST(LoginInteractor, SessionTokenStoredInRepo) {
    MockUserRepository userRepo;
    MockSessionRepository sessionRepo;
    registerUser(userRepo, "alice", "pass1234");
    LoginInteractor interactor(userRepo, sessionRepo);
    std::string token = interactor.execute("alice", "pass1234");
    EXPECT_EQ(sessionRepo.sessions.count(token), 1u);
}

// ─── LogoutInteractor ─────────────────────────────────────────────────────────

TEST(LogoutInteractor, DeletesSessionByToken) {
    MockSessionRepository sessionRepo;
    Session s; s.sessionId = 1; s.userId = 1; s.token = "abc123"; s.expiresAt = "2099-01-01T00:00:00Z";
    sessionRepo.sessions["abc123"] = s;
    LogoutInteractor interactor(sessionRepo);
    interactor.execute("abc123");
    EXPECT_TRUE(sessionRepo.deleteByTokenCalled);
    EXPECT_EQ(sessionRepo.sessions.count("abc123"), 0u);
}

// ─── ValidateTokenInteractor ──────────────────────────────────────────────────

TEST(ValidateTokenInteractor, ValidTokenReturnsUserId) {
    MockSessionRepository sessionRepo;
    Session s; s.sessionId = 1; s.userId = 42; s.token = "tok"; s.expiresAt = getExpiryTimestamp(24);
    sessionRepo.sessions["tok"] = s;
    ValidateTokenInteractor interactor(sessionRepo);
    EXPECT_EQ(interactor.execute("tok"), 42);
}

TEST(ValidateTokenInteractor, UnknownTokenReturnsMinusOne) {
    MockSessionRepository sessionRepo;
    ValidateTokenInteractor interactor(sessionRepo);
    EXPECT_EQ(interactor.execute("no_such_token"), -1);
}

TEST(ValidateTokenInteractor, ExpiredTokenReturnsMinusOne) {
    MockSessionRepository sessionRepo;
    Session s; s.sessionId = 1; s.userId = 5; s.token = "old"; s.expiresAt = "2000-01-01T00:00:00Z";
    sessionRepo.sessions["old"] = s;
    ValidateTokenInteractor interactor(sessionRepo);
    EXPECT_EQ(interactor.execute("old"), -1);
}
