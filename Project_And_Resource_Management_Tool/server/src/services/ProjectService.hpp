#pragma once

#include "../dto/ProjectDtos.hpp"
#include "../models/Project.hpp"
#include "../models/Milestone.hpp"
#include "../repositories/IProjectRepository.hpp"
#include "../repositories/IMilestoneRepository.hpp"

#include <memory>
#include <vector>

class ProjectService {
public:
    ProjectService(
        std::shared_ptr<IProjectRepository>   projectRepository,
        std::shared_ptr<IMilestoneRepository> milestoneRepository
    );

    int                    createProject(const CreateProjectRequest& request);
    std::vector<Project>   getAllProjects();
    void                   updateProject(int projectId, const UpdateProjectRequest& request);

    int                    createMilestone(int projectId, const CreateMilestoneRequest& request);
    std::vector<Milestone> getMilestones(int projectId);
    void                   updateMilestone(int projectId, int milestoneId,
                                           const UpdateMilestoneRequest& request);

private:
    std::shared_ptr<IProjectRepository>   projectRepository;
    std::shared_ptr<IMilestoneRepository> milestoneRepository;

    static const std::vector<std::string> VALID_PROJECT_STATUSES;
    static const std::vector<std::string> VALID_MILESTONE_STATUSES;

    void validateProjectStatus(const std::string& status) const;
    void validateMilestoneStatus(const std::string& status) const;
};
