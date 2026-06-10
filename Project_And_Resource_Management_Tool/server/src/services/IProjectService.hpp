#pragma once

#include "../dto/ProjectDtos.hpp"
#include "../models/Project.hpp"
#include "../models/Milestone.hpp"

#include <vector>

class IProjectService {
public:
    virtual ~IProjectService() = default;

    virtual Project                createProject(const CreateProjectRequest& request)                 = 0;
    virtual std::vector<Project>   getAllProjects()                                                   = 0;
    virtual std::vector<Project>   getProjectsByManagerId(int managerId)                             = 0;
    virtual Project                getProjectById(int projectId)                                     = 0;
    virtual Project                updateProject(int projectId,
                                                 const UpdateProjectRequest& request)                 = 0;

    virtual std::vector<Milestone> createMilestone(int projectId,
                                                    const CreateMilestoneRequest& request)            = 0;
    virtual std::vector<Milestone> getMilestones(int projectId)                                       = 0;
    virtual std::vector<Milestone> updateMilestone(int projectId, int milestoneId,
                                                    const UpdateMilestoneRequest& request)            = 0;
};
