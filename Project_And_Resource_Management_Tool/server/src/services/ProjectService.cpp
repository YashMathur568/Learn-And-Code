#include "ProjectService.hpp"
#include "../utils/AppException.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <algorithm>

const std::vector<std::string> ProjectService::VALID_PROJECT_STATUSES   = {"PLANNED", "ACTIVE", "ON_HOLD"};
const std::vector<std::string> ProjectService::VALID_MILESTONE_STATUSES = {"NOT_STARTED", "IN_PROGRESS", "DONE"};

ProjectService::ProjectService(
    std::shared_ptr<IProjectRepository>   projectRepository,
    std::shared_ptr<IMilestoneRepository> milestoneRepository,
    std::shared_ptr<IEmployeeRepository>  employeeRepository
)
    : projectRepository(std::move(projectRepository))
    , milestoneRepository(std::move(milestoneRepository))
    , employeeRepository(std::move(employeeRepository)) {}

void ProjectService::validateProjectStatus(const std::string& status) const {
    bool statusValid = std::any_of(
        VALID_PROJECT_STATUSES.begin(), VALID_PROJECT_STATUSES.end(),
        [&status](const std::string& validStatus) { return validStatus == status; }
    );
    if (!statusValid) {
        throw ValidationException("Project status must be PLANNED, ACTIVE, or ON_HOLD.");
    }
}

void ProjectService::validateMilestoneStatus(const std::string& status) const {
    bool statusValid = std::any_of(
        VALID_MILESTONE_STATUSES.begin(), VALID_MILESTONE_STATUSES.end(),
        [&status](const std::string& validStatus) { return validStatus == status; }
    );
    if (!statusValid) {
        throw ValidationException("Milestone status must be NOT_STARTED, IN_PROGRESS, or DONE.");
    }
}

Project ProjectService::createProject(const CreateProjectRequest& request) {
    if (request.name.empty() || request.startDate.empty() || request.endDate.empty()) {
        throw ValidationException("name, startDate, and endDate are required.");
    }

    validateProjectStatus(request.status);

    if (!employeeRepository->findById(request.managerId).has_value()) {
        throw NotFoundException(
            "Employee with ID " + std::to_string(request.managerId) +
            " not found or is inactive. Cannot assign as project manager."
        );
    }

    if (request.startDate >= request.endDate) {
        throw ValidationException("startDate must be before endDate.");
    }

    Project project;
    project.name        = request.name;
    project.description = request.description;
    project.startDate   = request.startDate;
    project.endDate     = request.endDate;
    project.status      = request.status;
    project.managerId   = request.managerId;

    const int newProjectId = projectRepository->create(project);
    return projectRepository->findById(newProjectId).value();
}

std::vector<Project> ProjectService::getAllProjects() {
    return projectRepository->findAll();
}

std::vector<Project> ProjectService::getProjectsByManagerId(int managerId) {
    return projectRepository->findByManagerId(managerId);
}

Project ProjectService::getProjectById(int projectId) {
    auto optionalProject = projectRepository->findById(projectId);
    if (!optionalProject.has_value()) {
        throw NotFoundException("Project with ID " + std::to_string(projectId) + " not found.");
    }
    return optionalProject.value();
}

Project ProjectService::updateProject(int projectId, const UpdateProjectRequest& request) {
    auto optionalProject = projectRepository->findById(projectId);
    if (!optionalProject.has_value()) {
        throw NotFoundException("Project with ID " + std::to_string(projectId) + " not found.");
    }

    if (request.name.empty() || request.startDate.empty() || request.endDate.empty()) {
        throw ValidationException("name, startDate, and endDate are required.");
    }

    validateProjectStatus(request.status);

    if (!employeeRepository->findById(request.managerId).has_value()) {
        throw NotFoundException(
            "Employee with ID " + std::to_string(request.managerId) + " not found or is inactive."
        );
    }

    if (request.startDate >= request.endDate) {
        throw ValidationException("startDate must be before endDate.");
    }

    Project updated        = optionalProject.value();
    updated.name           = request.name;
    updated.description    = request.description;
    updated.startDate      = request.startDate;
    updated.endDate        = request.endDate;
    updated.status         = request.status;
    updated.managerId      = request.managerId;

    projectRepository->update(updated);
    return projectRepository->findById(projectId).value();
}

std::vector<Milestone> ProjectService::createMilestone(int projectId, const CreateMilestoneRequest& request) {
    auto optionalProject = projectRepository->findById(projectId);
    if (!optionalProject.has_value()) {
        throw NotFoundException("Project with ID " + std::to_string(projectId) + " not found.");
    }

    if (request.title.empty() || request.dueDate.empty()) {
        throw ValidationException("title and dueDate are required.");
    }

    validateMilestoneStatus(request.status);

    Milestone milestone;
    milestone.projectId = projectId;
    milestone.title     = request.title;
    milestone.dueDate   = request.dueDate;
    milestone.status    = request.status;

    milestoneRepository->create(milestone);
    return milestoneRepository->findByProjectId(projectId);
}

std::vector<Milestone> ProjectService::getMilestones(int projectId) {
    auto optionalProject = projectRepository->findById(projectId);
    if (!optionalProject.has_value()) {
        throw NotFoundException("Project with ID " + std::to_string(projectId) + " not found.");
    }
    return milestoneRepository->findByProjectId(projectId);
}

std::vector<Milestone> ProjectService::updateMilestone(
    int projectId,
    int milestoneId,
    const UpdateMilestoneRequest& request
) {
    if (!milestoneRepository->milestoneBelongsToProject(milestoneId, projectId)) {
        throw NotFoundException("Milestone not found for this project.");
    }

    if (request.title.empty() || request.dueDate.empty()) {
        throw ValidationException("title and dueDate are required.");
    }

    validateMilestoneStatus(request.status);

    Milestone updated;
    updated.milestoneId = milestoneId;
    updated.projectId   = projectId;
    updated.title       = request.title;
    updated.dueDate     = request.dueDate;
    updated.status      = request.status;

    milestoneRepository->update(updated);
    return milestoneRepository->findByProjectId(projectId);
}
