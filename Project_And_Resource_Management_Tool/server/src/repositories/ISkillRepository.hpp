#pragma once

#include "../models/EmployeeSkill.hpp"
#include <optional>
#include <string>
#include <vector>

class ISkillRepository {
public:
    virtual ~ISkillRepository() = default;

    virtual std::vector<EmployeeSkill> findByEmployeeId(int employeeId)        = 0;
    virtual std::optional<EmployeeSkill> findById(int skillId)                 = 0;
    virtual int  create(const EmployeeSkill& skill)                            = 0;
    virtual void update(const EmployeeSkill& skill)                            = 0;
    virtual void remove(int skillId)                                           = 0;
    virtual bool skillBelongsToEmployee(int skillId, int employeeId)           = 0;
};
