#pragma once

#include "ResourceSkill.hpp"
#include <optional>
#include <string>
#include <vector>

class ISkillRepository {
public:
    virtual ~ISkillRepository() = default;

    virtual std::vector<ResourceSkill>   findByUserId(int userId)                        = 0;
    virtual std::optional<ResourceSkill> findById(int skillId)                          = 0;
    virtual int  create(const ResourceSkill& skill)                                     = 0;
    virtual void update(const ResourceSkill& skill)                                     = 0;
    virtual void remove(int skillId)                                                    = 0;
    virtual bool skillBelongsToUser(int skillId, int userId)                            = 0;
};
