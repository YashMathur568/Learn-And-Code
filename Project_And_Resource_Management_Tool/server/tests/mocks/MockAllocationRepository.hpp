#pragma once
#include "../../src/repositories/IAllocationRepository.hpp"
#include <gmock/gmock.h>

class MockAllocationRepository : public IAllocationRepository {
public:
    MOCK_METHOD(std::optional<Allocation>, findById,                 (int allocationId), (override));
    MOCK_METHOD(std::vector<Allocation>,   findByUserId,             (int userId), (override));
    MOCK_METHOD(std::vector<Allocation>,   findActiveByUserId,       (int userId), (override));
    MOCK_METHOD(std::vector<Allocation>,   findActiveByProjectId,    (int projectId), (override));
    MOCK_METHOD(int,                       getTotalActiveUtilisation,(int userId), (override));
    MOCK_METHOD(bool,                      isActivelyAllocated,      (int userId, int projectId), (override));
    MOCK_METHOD(int,                       create,                   (const Allocation& allocation), (override));
    MOCK_METHOD(void,                      end,                      (int allocationId), (override));
    MOCK_METHOD(void,                      endAllByUser,             (int userId), (override));
};
