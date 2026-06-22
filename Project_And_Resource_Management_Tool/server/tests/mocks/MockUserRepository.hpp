#pragma once
#include "IUserRepository.hpp"
#include <gmock/gmock.h>

class MockUserRepository : public IUserRepository {
public:
    MOCK_METHOD(std::optional<User>, findByUsername,       (const std::string& username), (override));
    MOCK_METHOD(std::optional<User>, findById,             (int userId), (override));
    MOCK_METHOD(std::vector<User>,   findAll,              (), (override));
    MOCK_METHOD(int,                 create,               (const User& user), (override));
    MOCK_METHOD(void,                updatePasswordHash,   (int userId, const std::string& newHash), (override));
    MOCK_METHOD(void,                refreshPasswordExpiry,(int userId), (override));
    MOCK_METHOD(void,                expirePasswordNow,    (int userId), (override));
    MOCK_METHOD(void,                setActiveStatus,      (int userId, bool active), (override));
    MOCK_METHOD(void,                updateRole,           (int userId, const std::string& newRole), (override));
    MOCK_METHOD(int,                 countActiveAdmins,    (), (override));
    MOCK_METHOD(bool,                existsByUsername,     (const std::string& username), (override));
    MOCK_METHOD(bool,                existsByEmail,        (const std::string& email), (override));
};
