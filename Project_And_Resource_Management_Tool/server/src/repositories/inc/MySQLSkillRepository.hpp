#pragma once

#include "ISkillRepository.hpp"
#include "DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLSkillRepository : public ISkillRepository {
public:
    MySQLSkillRepository() = default;

    std::vector<ResourceSkill>   findByUserId(int userId)              override;
    std::optional<ResourceSkill> findById(int skillId)                 override;
    int                          create(const ResourceSkill& skill)    override;
    void                         update(const ResourceSkill& skill)    override;
    void                         remove(int skillId)                   override;
    bool                         skillBelongsToUser(int skillId,
                                                    int userId)        override;

private:
    static ResourceSkill mapRowToSkill(sql::ResultSet* resultSet);
};
