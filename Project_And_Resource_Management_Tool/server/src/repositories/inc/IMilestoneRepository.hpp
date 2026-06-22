#pragma once

#include "Milestone.hpp"
#include <optional>
#include <vector>

class IMilestoneRepository {
public:
    virtual ~IMilestoneRepository() = default;

    virtual std::optional<Milestone> findById(int milestoneId)                        = 0;
    virtual std::vector<Milestone>   findByProjectId(int projectId)                   = 0;
    virtual int                      create(const Milestone& milestone)               = 0;
    virtual void                     update(const Milestone& milestone)               = 0;
    virtual bool                     milestoneBelongsToProject(int milestoneId,
                                                               int projectId)         = 0;
};
