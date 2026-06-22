#pragma once

#include "AdminDtos.hpp"
#include "Resource.hpp"
#include "ResourceSkill.hpp"
#include "Allocation.hpp"

#include <vector>

class IResourceService {
public:
    virtual ~IResourceService() = default;

    virtual std::vector<Resource>    getAllResources()                                                     = 0;
    virtual std::vector<Resource>    getByManagerId(int managerId)                                        = 0;
    virtual Resource                 getById(int resourceId)                                              = 0;
    virtual Resource                 updateResource(int resourceId, const UpdateResourceRequest& request) = 0;
    virtual std::vector<Allocation>  deactivateResource(int resourceId)                                   = 0;
    virtual void                     assignManager(int resourceId, const AssignManagerRequest& request)   = 0;
    virtual Resource                 changeRole(int resourceId,
                                               const ChangeRoleRequest& request,
                                               int requesterId)                                            = 0;

    virtual std::vector<ResourceSkill> getSkills(int resourceId)                                          = 0;
    virtual ResourceSkill              addSkill(int resourceId, const SkillRequest& request)              = 0;
    virtual void                       updateSkill(int resourceId, int skillId,
                                                   const SkillRequest& request)                           = 0;
    virtual void                       removeSkill(int resourceId, int skillId)                           = 0;
};
