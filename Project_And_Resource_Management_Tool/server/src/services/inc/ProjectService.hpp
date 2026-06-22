#pragma once

#include "IProjectService.hpp"
#include "IProjectRepository.hpp"
#include "IMilestoneRepository.hpp"
#include "IResourceRepository.hpp"

#include <memory>
#include <vector>

class ProjectService : public IProjectService {
public:
    ProjectService(
        std::shared_ptr<IProjectRepository>   projectRepository,
        std::shared_ptr<IMilestoneRepository> milestoneRepository,
        std::shared_ptr<IResourceRepository>  resourceRepository
    );

    Project                createProject(const CreateProjectRequest& request)                 override;
    std::vector<Project>   getAllProjects()                                                   override;
    std::vector<Project>   getProjectsByManagerId(int managerId)                             override;
    Project                getProjectById(int projectId)                                     override;
    Project                updateProject(int projectId,
                                         const UpdateProjectRequest& request)                 override;

    std::vector<Milestone> createMilestone(int projectId,
                                            const CreateMilestoneRequest& request)            override;
    std::vector<Milestone> getMilestones(int projectId)                                       override;
    std::vector<Milestone> updateMilestone(int projectId, int milestoneId,
                                            const UpdateMilestoneRequest& request)            override;

private:
    std::shared_ptr<IProjectRepository>   projectRepository;
    std::shared_ptr<IMilestoneRepository> milestoneRepository;
    std::shared_ptr<IResourceRepository>  resourceRepository;

    void validateProjectStatus(const std::string& status) const;
    void validateMilestoneStatus(const std::string& status) const;

};
