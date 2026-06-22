#pragma once

#include "Allocation.hpp"
#include <optional>
#include <vector>

class IAllocationRepository {
public:
    virtual ~IAllocationRepository() = default;

    virtual std::optional<Allocation> findById(int allocationId)                       = 0;
    virtual std::vector<Allocation>   findByUserId(int userId)                         = 0;
    virtual std::vector<Allocation>   findActiveByUserId(int userId)                   = 0;
    virtual std::vector<Allocation>   findActiveByProjectId(int projectId)             = 0;
    virtual int                       getTotalActiveUtilisation(int userId)            = 0;
    virtual bool                      wasAllocatedDuringWeek(int userId, int projectId, const std::string& weekStart) = 0;
    virtual std::string               getProjectStatus(int projectId)                 = 0;
    virtual int                       create(const Allocation& allocation)             = 0;
    virtual void                      end(int allocationId)                            = 0;
    virtual void                      endAllByUser(int userId)                         = 0;
};
