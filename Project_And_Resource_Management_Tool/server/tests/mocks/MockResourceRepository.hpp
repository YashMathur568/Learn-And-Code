#pragma once
#include "IResourceRepository.hpp"
#include <gmock/gmock.h>

class MockResourceRepository : public IResourceRepository {
public:
    MOCK_METHOD(std::optional<Resource>, findById,          (int userId), (override));
    MOCK_METHOD(std::vector<Resource>,   findAll,           (), (override));
    MOCK_METHOD(std::vector<Resource>,   findAllActive,     (), (override));
    MOCK_METHOD(std::vector<Resource>,   findByManagerId,   (int managerUserId), (override));
    MOCK_METHOD(int,                     create,            (const Resource& resource), (override));
    MOCK_METHOD(void,                    update,            (const Resource& resource), (override));
    MOCK_METHOD(void,                    setActiveStatus,   (int userId, bool active), (override));
    MOCK_METHOD(void,                    setStatus,         (int userId, const std::string& status), (override));
    MOCK_METHOD(void,                    assignManager,     (int userId, int managerUserId), (override));
    MOCK_METHOD(void,                    setFrozen,         (int userId, bool frozen), (override));
    MOCK_METHOD(bool,                    isFrozen,          (int userId), (override));
    MOCK_METHOD(bool,                    hasActiveAllocations, (int userId), (override));
    MOCK_METHOD(bool,                    existsByUserId,    (int userId), (override));
    MOCK_METHOD(void,                    deleteResourceStatus, (int userId), (override));
    MOCK_METHOD(void,                    createResourceStatus, (int userId), (override));
    MOCK_METHOD(int,                     countManagedProjects, (int userId), (override));
};
