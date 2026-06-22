#pragma once

#include "IAllocationService.hpp"
#include "IAllocationRepository.hpp"
#include "IResourceRepository.hpp"
#include "IProjectRepository.hpp"

#include <memory>
#include <vector>

class AllocationService : public IAllocationService {
public:
    AllocationService(
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IResourceRepository>   resourceRepository,
        std::shared_ptr<IProjectRepository>    projectRepository
    );

    Allocation              createAllocation(int managerUserId,
                                             const CreateAllocationRequest& request) override;
    Allocation              endAllocation(int allocationId, int managerUserId)       override;
    std::vector<Allocation> getByUserId(int userId)                                  override;
    std::vector<Allocation> getActiveByUserId(int userId)                            override;
    std::vector<Allocation> getActiveByProjectId(int projectId)                      override;
    int                     getTotalUtilisation(int userId)                          override;

private:
    std::shared_ptr<IAllocationRepository> allocationRepository;
    std::shared_ptr<IResourceRepository>   resourceRepository;
    std::shared_ptr<IProjectRepository>    projectRepository;
};
