#pragma once

#include "IMilestoneRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLMilestoneRepository : public IMilestoneRepository {
public:
    MySQLMilestoneRepository() = default;

    std::optional<Milestone> findById(int milestoneId)                       override;
    std::vector<Milestone>   findByProjectId(int projectId)                  override;
    int                      create(const Milestone& milestone)              override;
    void                     update(const Milestone& milestone)              override;
    bool                     milestoneBelongsToProject(int milestoneId,
                                                       int projectId)        override;

private:
    static Milestone mapRowToMilestone(sql::ResultSet* resultSet);
};
