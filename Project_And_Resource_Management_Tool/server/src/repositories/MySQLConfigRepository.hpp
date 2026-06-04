#pragma once

#include "IConfigRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLConfigRepository : public IConfigRepository {
public:
    MySQLConfigRepository() = default;

    std::optional<SystemConfig> findByKey(const std::string& key) override;
    std::vector<SystemConfig>   findAll()                         override;
    void                        upsert(const std::string& key,
                                       const std::string& value)  override;

private:
    static SystemConfig mapRowToConfig(sql::ResultSet* resultSet);
};
