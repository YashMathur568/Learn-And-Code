#pragma once

#include "IResourceService.hpp"
#include "IResourceRepository.hpp"
#include "IUserRepository.hpp"
#include "ISkillRepository.hpp"
#include "IAllocationRepository.hpp"

#include <memory>
#include <vector>

class ResourceService : public IResourceService {
public:
    ResourceService(
        std::shared_ptr<IResourceRepository>  resourceRepository,
        std::shared_ptr<IUserRepository>      userRepository,
        std::shared_ptr<ISkillRepository>     skillRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository
    );

    std::vector<Resource>    getAllResources()                                                     override;
    std::vector<Resource>    getByManagerId(int managerId)                                        override;
    Resource                 getById(int resourceId)                                              override;
    Resource                 updateResource(int resourceId, const UpdateResourceRequest& request) override;
    std::vector<Allocation>  deactivateResource(int resourceId)                                   override;
    void                     assignManager(int resourceId, const AssignManagerRequest& request)   override;
    Resource                 changeRole(int resourceId,
                                        const ChangeRoleRequest& request,
                                        int requesterId)                                           override;

    std::vector<ResourceSkill> getSkills(int resourceId)                                          override;
    ResourceSkill              addSkill(int resourceId, const SkillRequest& request)              override;
    void                       updateSkill(int resourceId, int skillId,
                                           const SkillRequest& request)                           override;
    void                       removeSkill(int resourceId, int skillId)                           override;

private:
    std::shared_ptr<IResourceRepository>  resourceRepository;
    std::shared_ptr<IUserRepository>      userRepository;
    std::shared_ptr<ISkillRepository>     skillRepository;
    std::shared_ptr<IAllocationRepository> allocationRepository;

    void validateSkillFields(const SkillRequest& request) const;
};
