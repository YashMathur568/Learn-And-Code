#pragma once

#include "../dto/AllocationDtos.hpp"

#include <vector>

class IAllocationService {
public:
    virtual ~IAllocationService() = default;

    virtual Allocation              createAllocation(int managerEmployeeId,
                                                     const CreateAllocationRequest& request) = 0;
    virtual Allocation              endAllocation(int allocationId, int managerEmployeeId)   = 0;
    virtual std::vector<Allocation> getByEmployeeId(int employeeId)                          = 0;
    virtual std::vector<Allocation> getActiveByEmployeeId(int employeeId)                    = 0;
    virtual std::vector<Allocation> getActiveByProjectId(int projectId)                      = 0;
    virtual int                     getTotalUtilisation(int employeeId)                      = 0;
};
