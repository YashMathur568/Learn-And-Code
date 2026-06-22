#pragma once

#include "AllocationDtos.hpp"

#include <vector>

class IAllocationService {
public:
    virtual ~IAllocationService() = default;

    virtual Allocation              createAllocation(int managerUserId,
                                                     const CreateAllocationRequest& request) = 0;
    virtual Allocation              endAllocation(int allocationId, int managerUserId)       = 0;
    virtual std::vector<Allocation> getByUserId(int userId)                                  = 0;
    virtual std::vector<Allocation> getActiveByUserId(int userId)                            = 0;
    virtual std::vector<Allocation> getActiveByProjectId(int projectId)                      = 0;
    virtual int                     getTotalUtilisation(int userId)                          = 0;
};
