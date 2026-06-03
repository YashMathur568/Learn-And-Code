#pragma once

#include "IUserRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLUserRepository : public IUserRepository {
public:
    MySQLUserRepository() = default;

    std::optional<User> findByUsername(const std::string& username)    override;
    std::optional<User> findById(int userId)                           override;
    void                updatePasswordHash(int userId,
                                           const std::string& newHash) override;
    void                setForcePwdChange(int userId, bool value)      override;

private:
    static User mapRowToUser(sql::ResultSet* resultSet);
};
