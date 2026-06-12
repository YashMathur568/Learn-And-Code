#pragma once
#include "../../src/repositories/ISkillRepository.hpp"
#include <gmock/gmock.h>

class MockSkillRepository : public ISkillRepository {
public:
    MOCK_METHOD(std::vector<EmployeeSkill>,   findByUserId,         (int userId), (override));
    MOCK_METHOD(std::optional<EmployeeSkill>, findById,             (int skillId), (override));
    MOCK_METHOD(int,                          create,               (const EmployeeSkill& skill), (override));
    MOCK_METHOD(void,                         update,               (const EmployeeSkill& skill), (override));
    MOCK_METHOD(void,                         remove,               (int skillId), (override));
    MOCK_METHOD(bool,                         skillBelongsToUser,   (int skillId, int userId), (override));
};
