#pragma once

#include "IProjectService.hpp"
#include "../repositories/IProjectRepository.hpp"
#include "../repositories/IMilestoneRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>
#include <vector>

class ProjectService : public IProjectService {
public:
    ProjectService(
        std::shared_ptr<IProjectRepository>   projectRepository,
        std::shared_ptr<IMilestoneRepository> milestoneRepository,
        std::shared_ptr<IEmployeeRepository>  employeeRepository
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
    std::shared_ptr<IEmployeeRepository>  employeeRepository;

    static const std::vector<std::string> VALID_PROJECT_STATUSES;
    static const std::vector<std::string> VALID_MILESTONE_STATUSES;

    void validateProjectStatus(const std::string& status) const;
    void validateMilestoneStatus(const std::string& status) const;
};
