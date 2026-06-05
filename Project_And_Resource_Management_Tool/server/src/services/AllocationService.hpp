#pragma once

#include "../dto/AllocationDtos.hpp"
#include "../models/Allocation.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IProjectRepository.hpp"

#include <memory>
#include <vector>

class AllocationService {
public:
    AllocationService(
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IEmployeeRepository>   employeeRepository,
        std::shared_ptr<IProjectRepository>    projectRepository
    );

    Allocation              createAllocation(int managerEmployeeId, const CreateAllocationRequest& request);
    Allocation              endAllocation(int allocationId, int managerEmployeeId);
    std::vector<Allocation> getByEmployeeId(int employeeId);
    std::vector<Allocation> getActiveByEmployeeId(int employeeId);
    std::vector<Allocation> getActiveByProjectId(int projectId);
    int                     getTotalUtilisation(int employeeId);

private:
    std::shared_ptr<IAllocationRepository> allocationRepository;
    std::shared_ptr<IEmployeeRepository>   employeeRepository;
    std::shared_ptr<IProjectRepository>    projectRepository;
};
