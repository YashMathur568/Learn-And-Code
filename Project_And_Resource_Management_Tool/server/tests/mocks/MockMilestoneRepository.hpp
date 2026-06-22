#pragma once
#include "IMilestoneRepository.hpp"
#include <gmock/gmock.h>

class MockMilestoneRepository : public IMilestoneRepository {
public:
    MOCK_METHOD(std::optional<Milestone>, findById,               (int milestoneId), (override));
    MOCK_METHOD(std::vector<Milestone>,   findByProjectId,        (int projectId), (override));
    MOCK_METHOD(int,                      create,                 (const Milestone& milestone), (override));
    MOCK_METHOD(void,                     update,                 (const Milestone& milestone), (override));
    MOCK_METHOD(bool,                     milestoneBelongsToProject, (int milestoneId, int projectId), (override));
};
