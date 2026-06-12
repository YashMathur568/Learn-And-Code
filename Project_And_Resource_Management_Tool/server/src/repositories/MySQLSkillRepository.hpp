#pragma once

#include "ISkillRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLSkillRepository : public ISkillRepository {
public:
    MySQLSkillRepository() = default;

    std::vector<EmployeeSkill>   findByUserId(int userId)              override;
    std::optional<EmployeeSkill> findById(int skillId)                 override;
    int                          create(const EmployeeSkill& skill)    override;
    void                         update(const EmployeeSkill& skill)    override;
    void                         remove(int skillId)                   override;
    bool                         skillBelongsToUser(int skillId,
                                                    int userId)        override;

private:
    static EmployeeSkill mapRowToSkill(sql::ResultSet* resultSet);
};
