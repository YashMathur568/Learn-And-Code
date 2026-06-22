#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "UserService.hpp"
#include "AppException.hpp"
#include "mocks/MockUserRepository.hpp"
#include "mocks/MockResourceRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;


static User makeUser(int id = 1, bool active = true) {
    User user;
    user.userId   = id;
    user.fullName = "Test User";
    user.email    = "test@example.com";
    user.username = "testuser";
    user.role     = "RESOURCE";
    user.isActive = active;
    return user;
}

static Resource makeResource(int id = 1) {
    Resource res;
    res.userId   = id;
    res.fullName = "Test User";
    res.isActive = true;
    return res;
}

static CreateUserRequest validResourceRequest() {
    CreateUserRequest req;
    req.fullName     = "John Doe";
    req.email        = "john@example.com";
    req.username     = "jdoe";
    req.tempPassword = "Password1!";
    req.role         = "RESOURCE";
    req.department   = "Engineering";
    req.designation  = "Junior";
    return req;
}

class UserServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockUserRepository>     userRepo;
    std::shared_ptr<MockResourceRepository> resRepo;
    std::unique_ptr<UserService>            service;

    void SetUp() override {
        userRepo = std::make_shared<NiceMock<MockUserRepository>>();
        resRepo  = std::make_shared<NiceMock<MockResourceRepository>>();
        service  = std::make_unique<UserService>(userRepo, resRepo);
    }
};


// ── createUser ────────────────────────────────────────────────────────────────

TEST_F(UserServiceTest, CreateUser_EmptyFullName_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.fullName = "";
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_InvalidRole_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.role = "SUPERUSER";
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_UsernameTaken_ThrowsConflict) {
    EXPECT_CALL(*userRepo, existsByUsername("jdoe")).WillOnce(Return(true));
    EXPECT_THROW(service->createUser(validResourceRequest()), ConflictException);
}

TEST_F(UserServiceTest, CreateUser_EmailTaken_ThrowsConflict) {
    EXPECT_CALL(*userRepo, existsByUsername(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, existsByEmail(_)).WillOnce(Return(true));
    EXPECT_THROW(service->createUser(validResourceRequest()), ConflictException);
}

TEST_F(UserServiceTest, CreateUser_ResourceMissingDepartment_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.department = "";
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_ResourceMissingDesignation_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.designation = "";
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_ManagerMissingDesignation_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.role        = "MANAGER";
    req.designation = "";
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_WeakPassword_ThrowsValidation) {
    CreateUserRequest req = validResourceRequest();
    req.tempPassword = "weak";
    EXPECT_CALL(*userRepo, existsByUsername(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, existsByEmail(_)).WillOnce(Return(false));
    EXPECT_THROW(service->createUser(req), ValidationException);
}

TEST_F(UserServiceTest, CreateUser_AdminRole_DoesNotRequireDepartment) {
    CreateUserRequest req = validResourceRequest();
    req.role        = "ADMIN";
    req.department  = "";
    req.designation = "";

    User created = makeUser(10);
    created.role = "ADMIN";
    EXPECT_CALL(*userRepo, existsByUsername(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, existsByEmail(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, create(_)).WillOnce(Return(10));
    EXPECT_CALL(*userRepo, findById(10)).WillOnce(Return(created));
    EXPECT_CALL(*resRepo, create(_)).WillOnce(Return(10));
    EXPECT_CALL(*resRepo, findById(10)).WillOnce(Return(std::make_optional(makeResource(10))));

    CreatedUserResult result = service->createUser(req);
    EXPECT_EQ(result.user.userId, 10);
}

TEST_F(UserServiceTest, CreateUser_ResourceRole_Success) {
    User created = makeUser(5);
    EXPECT_CALL(*userRepo, existsByUsername(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, existsByEmail(_)).WillOnce(Return(false));
    EXPECT_CALL(*userRepo, create(_)).WillOnce(Return(5));
    EXPECT_CALL(*userRepo, findById(5)).WillOnce(Return(created));
    EXPECT_CALL(*resRepo, create(_)).WillOnce(Return(5));
    EXPECT_CALL(*resRepo, findById(5)).WillOnce(Return(std::make_optional(makeResource(5))));

    CreatedUserResult result = service->createUser(validResourceRequest());
    EXPECT_EQ(result.user.userId, 5);
}


// ── deactivateUser ────────────────────────────────────────────────────────────

TEST_F(UserServiceTest, DeactivateUser_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->deactivateUser(99), NotFoundException);
}

TEST_F(UserServiceTest, DeactivateUser_AlreadyInactive_ThrowsValidation) {
    User user = makeUser(1, false);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_THROW(service->deactivateUser(1), ValidationException);
}

TEST_F(UserServiceTest, DeactivateUser_Success) {
    User user = makeUser(1, true);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_CALL(*userRepo, setActiveStatus(1, false)).Times(1);
    EXPECT_NO_THROW(service->deactivateUser(1));
}


// ── reactivateUser ────────────────────────────────────────────────────────────

TEST_F(UserServiceTest, ReactivateUser_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->reactivateUser(99), NotFoundException);
}

TEST_F(UserServiceTest, ReactivateUser_AlreadyActive_ThrowsValidation) {
    User user = makeUser(1, true);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_THROW(service->reactivateUser(1), ValidationException);
}

TEST_F(UserServiceTest, ReactivateUser_Success) {
    User user = makeUser(1, false);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_CALL(*userRepo, setActiveStatus(1, true)).Times(1);
    EXPECT_NO_THROW(service->reactivateUser(1));
}


// ── resetPassword ─────────────────────────────────────────────────────────────

TEST_F(UserServiceTest, ResetPassword_UserNotFound_ThrowsNotFound) {
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    ResetPasswordRequest req{"NewPass1!"};
    EXPECT_THROW(service->resetPassword(99, req), NotFoundException);
}

TEST_F(UserServiceTest, ResetPassword_WeakPassword_ThrowsValidation) {
    User user = makeUser(1);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    ResetPasswordRequest req{"weak"};
    EXPECT_THROW(service->resetPassword(1, req), ValidationException);
}

TEST_F(UserServiceTest, ResetPassword_Success) {
    User user = makeUser(1);
    EXPECT_CALL(*userRepo, findById(1)).WillOnce(Return(user));
    EXPECT_CALL(*userRepo, updatePasswordHash(1, _)).Times(1);
    EXPECT_CALL(*userRepo, expirePasswordNow(1)).Times(1);
    ResetPasswordRequest req{"NewPass1!"};
    EXPECT_NO_THROW(service->resetPassword(1, req));
}
