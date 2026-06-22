#pragma once
#include "IProjectRepository.hpp"
#include <gmock/gmock.h>

class MockProjectRepository : public IProjectRepository {
public:
    MOCK_METHOD(std::optional<Project>, findById,      (int projectId), (override));
    MOCK_METHOD(std::vector<Project>,   findAll,       (), (override));
    MOCK_METHOD(std::vector<Project>,   findByManagerId, (int managerId), (override));
    MOCK_METHOD(int,                    create,        (const Project& project), (override));
    MOCK_METHOD(void,                   update,        (const Project& project), (override));
    MOCK_METHOD(void,                   updateHealth,  (int projectId, const std::string& health), (override));
};
