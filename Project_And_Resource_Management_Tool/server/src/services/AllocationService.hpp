#pragma once

#include "IAllocationService.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IProjectRepository.hpp"

#include <memory>
#include <vector>

class AllocationService : public IAllocationService {
public:
    AllocationService(
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IEmployeeRepository>   employeeRepository,
        std::shared_ptr<IProjectRepository>    projectRepository
    );

    Allocation              createAllocation(int managerEmployeeId,
                                             const CreateAllocationRequest& request) override;
    Allocation              endAllocation(int allocationId, int managerEmployeeId)   override;
    std::vector<Allocation> getByEmployeeId(int employeeId)                          override;
    std::vector<Allocation> getActiveByEmployeeId(int employeeId)                    override;
    std::vector<Allocation> getActiveByProjectId(int projectId)                      override;
    int                     getTotalUtilisation(int employeeId)                      override;

private:
    std::shared_ptr<IAllocationRepository> allocationRepository;
    std::shared_ptr<IEmployeeRepository>   employeeRepository;
    std::shared_ptr<IProjectRepository>    projectRepository;
};
