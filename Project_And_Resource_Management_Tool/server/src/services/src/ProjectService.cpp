#include "ProjectService.hpp"
#include "AppException.hpp"
#include "DateUtils.hpp"
#include "IResourceRepository.hpp"
#include "ProjectStatus.hpp"
#include "MilestoneStatus.hpp"

ProjectService::ProjectService(
    std::shared_ptr<IProjectRepository>   projectRepository,
    std::shared_ptr<IMilestoneRepository> milestoneRepository,
    std::shared_ptr<IResourceRepository>  resourceRepository
)
    : projectRepository(std::move(projectRepository))
    , milestoneRepository(std::move(milestoneRepository))
    , resourceRepository(std::move(resourceRepository)) {}

void ProjectService::validateProjectStatus(const std::string& status) const {
    if (!ProjectStatus::isValid(status)) {
        throw ValidationException("Project status must be PLANNED, ACTIVE, or ON_HOLD.");
    }
}

void ProjectService::validateMilestoneStatus(const std::string& status) const {
    if (!MilestoneStatus::isValid(status)) {
        throw ValidationException("Milestone status must be NOT_STARTED, IN_PROGRESS, or DONE.");
    }
}

Project ProjectService::createProject(const CreateProjectRequest& request) {
    if (request.name.empty() || request.startDate.empty() || request.endDate.empty()) {
        throw ValidationException("name, startDate, and endDate are required.");
    }

    validateProjectStatus(request.status);

    auto optionalManager = resourceRepository->findById(request.managerId);
    if (!optionalManager.has_value()) {
        throw NotFoundException(
            "Resource with ID " + std::to_string(request.managerId) +
            " not found or is inactive. Cannot assign as project manager."
        );
    }


    if (optionalManager.value().role != "MANAGER") {
        throw ValidationException(
            "User " + optionalManager.value().fullName +
            " (ID " + std::to_string(request.managerId) +
            ") is not a Manager. Only users with MANAGER role can be assigned to projects."
        );
    }

    if (request.startDate >= request.endDate) {
        throw ValidationException("startDate must be before endDate.");
    }
    if (!isValidIsoDate(request.startDate)) {
        throw ValidationException("startDate is not a valid date. Expected YYYY-MM-DD.");
    }
    if (!isValidIsoDate(request.endDate)) {
        throw ValidationException("endDate is not a valid date. Expected YYYY-MM-DD.");
    }

    Project project;
    project.name              = request.name;
    project.description       = request.description;
    project.startDate         = request.startDate;
    project.endDate           = request.endDate;
    project.status            = request.status;
    project.managerId         = request.managerId;
    project.totalStoryPoints  = request.totalStoryPoints;

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

    if (!request.status.empty()) validateProjectStatus(request.status);

    if (request.managerId != 0) {
        auto optionalManager = resourceRepository->findById(request.managerId);
        if (!optionalManager.has_value()) {
            throw NotFoundException(
                "Resource with ID " + std::to_string(request.managerId) + " not found or is inactive."
            );
        }


        if (optionalManager.value().role != "MANAGER") {
            throw ValidationException(
                "User " + optionalManager.value().fullName +
                " (ID " + std::to_string(request.managerId) +
                ") is not a Manager. Only users with MANAGER role can be assigned to projects."
            );
        }
    }

    if (!request.startDate.empty() && !isValidIsoDate(request.startDate)) {
        throw ValidationException("startDate is not a valid date. Expected YYYY-MM-DD.");
    }
    if (!request.endDate.empty() && !isValidIsoDate(request.endDate)) {
        throw ValidationException("endDate is not a valid date. Expected YYYY-MM-DD.");
    }

    Project updated        = optionalProject.value();
    updated.name           = request.name.empty()      ? updated.name      : request.name;
    updated.description    = request.description.empty() ? updated.description : request.description;
    updated.startDate      = request.startDate.empty() ? updated.startDate : request.startDate;
    updated.endDate        = request.endDate.empty()   ? updated.endDate   : request.endDate;
    updated.status         = request.status.empty()    ? updated.status    : request.status;
    updated.managerId      = request.managerId == 0    ? updated.managerId : request.managerId;
    if (request.totalStoryPoints > 0) updated.totalStoryPoints = request.totalStoryPoints;

    if (updated.startDate >= updated.endDate) {
        throw ValidationException("startDate must be before endDate.");
    }

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

    if (!isValidIsoDate(request.dueDate)) {
        throw ValidationException("dueDate is not a valid date. Expected YYYY-MM-DD.");
    }
    if (request.dueDate < optionalProject->startDate || request.dueDate > optionalProject->endDate) {
        throw ValidationException(
            "dueDate must be within the project period ("
            + optionalProject->startDate + " to " + optionalProject->endDate + ")."
        );
    }

    Milestone milestone;
    milestone.projectId  = projectId;
    milestone.title      = request.title;
    milestone.dueDate    = request.dueDate;
    milestone.status     = request.status;
    milestone.storyPoints = request.storyPoints;

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

    const auto existing = milestoneRepository->findById(milestoneId);
    if (!existing.has_value()) throw NotFoundException("Milestone not found.");

    if (!request.status.empty()) validateMilestoneStatus(request.status);

    Milestone updated;
    updated.milestoneId = milestoneId;
    updated.projectId   = projectId;
    updated.title       = request.title.empty()   ? existing->title   : request.title;
    updated.dueDate     = request.dueDate.empty() ? existing->dueDate : request.dueDate;
    updated.status      = request.status.empty()  ? existing->status  : request.status;

    if (!request.dueDate.empty()) {
        if (!isValidIsoDate(updated.dueDate)) {
            throw ValidationException("dueDate is not a valid date. Expected YYYY-MM-DD.");
        }
        const auto project = projectRepository->findById(projectId);
        if (project.has_value() &&
            (updated.dueDate < project->startDate || updated.dueDate > project->endDate)) {
            throw ValidationException(
                "dueDate must be within the project period ("
                + project->startDate + " to " + project->endDate + ")."
            );
        }
    }

    milestoneRepository->update(updated);
    return milestoneRepository->findByProjectId(projectId);
}
