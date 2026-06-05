#pragma once

#include "../models/Allocation.hpp"
#include <optional>
#include <vector>

class IAllocationRepository {
public:
    virtual ~IAllocationRepository() = default;

    virtual std::optional<Allocation> findById(int allocationId)                         = 0;
    virtual std::vector<Allocation>   findByEmployeeId(int employeeId)                   = 0;
    virtual std::vector<Allocation>   findActiveByEmployeeId(int employeeId)             = 0;
    virtual std::vector<Allocation>   findActiveByProjectId(int projectId)               = 0;
    virtual int                       getTotalActiveUtilisation(int employeeId)          = 0;
    virtual bool                      isActivelyAllocated(int employeeId, int projectId) = 0;
    virtual int                       create(const Allocation& allocation)               = 0;
    virtual void                      end(int allocationId)                              = 0;
    virtual void                      endAllByEmployee(int employeeId)                    = 0;
};
