#pragma once

#include "IUserRepository.hpp"
#include "../utils/DatabasePool.hpp"
#include <vector>

namespace sql {
    class ResultSet;
}

class MySQLUserRepository : public IUserRepository {
public:
    MySQLUserRepository() = default;

    std::optional<User> findByUsername(const std::string& username)    override;
    std::optional<User> findById(int userId)                           override;
    std::vector<User>   findAll()                                      override;
    int                 create(const User& user)                       override;
    void                updatePasswordHash(int userId,
                                           const std::string& newHash) override;
    void                setForcePwdChange(int userId, bool value)      override;
    void                setActiveStatus(int userId, bool active)       override;
    bool                existsByUsername(const std::string& username)  override;
    bool                existsByEmail(const std::string& email)        override;

private:
    static User mapRowToUser(sql::ResultSet* resultSet);
};
