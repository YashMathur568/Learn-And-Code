#pragma once
#include "ISkillRepository.hpp"
#include <gmock/gmock.h>

class MockSkillRepository : public ISkillRepository {
public:
    MOCK_METHOD(std::vector<ResourceSkill>,   findByUserId,         (int userId), (override));
    MOCK_METHOD(std::optional<ResourceSkill>, findById,             (int skillId), (override));
    MOCK_METHOD(int,                          create,               (const ResourceSkill& skill), (override));
    MOCK_METHOD(void,                         update,               (const ResourceSkill& skill), (override));
    MOCK_METHOD(void,                         remove,               (int skillId), (override));
    MOCK_METHOD(bool,                         skillBelongsToUser,   (int skillId, int userId), (override));
};
