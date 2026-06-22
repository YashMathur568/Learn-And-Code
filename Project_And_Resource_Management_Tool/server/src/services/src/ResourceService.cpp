#include "ResourceService.hpp"
#include "AppException.hpp"
#include "SkillCategory.hpp"
#include "Proficiency.hpp"
#include "Designation.hpp"
#include "Department.hpp"
#include "ResourceStatus.hpp"

ResourceService::ResourceService(
    std::shared_ptr<IResourceRepository>  resourceRepository,
    std::shared_ptr<IUserRepository>      userRepository,
    std::shared_ptr<ISkillRepository>     skillRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository
)
    : resourceRepository(std::move(resourceRepository))
    , userRepository(std::move(userRepository))
    , skillRepository(std::move(skillRepository))
    , allocationRepository(std::move(allocationRepository)) {}

void ResourceService::validateSkillFields(const SkillRequest& request) const {
    if (request.skillName.empty()) {
        throw ValidationException("skillName is required.");
    }

    if (!SkillCategory::isValid(request.category)) {
        throw ValidationException("category must be one of: Backend, Frontend, DevOps, QA, Other.");
    }

    if (!Proficiency::isValid(request.proficiency)) {
        throw ValidationException("proficiency must be one of: Beginner, Intermediate, Advanced.");
    }
}

std::vector<Resource> ResourceService::getAllResources() {
    return resourceRepository->findAll();
}

std::vector<Resource> ResourceService::getByManagerId(int managerId) {
    return resourceRepository->findByManagerId(managerId);
}

Resource ResourceService::getById(int resourceId) {
    auto optionalResource = resourceRepository->findById(resourceId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(resourceId) + " not found.");
    }
    return optionalResource.value();
}

Resource ResourceService::updateResource(int resourceId, const UpdateResourceRequest& request) {
    if (request.fullName.empty() || request.email.empty()) {
        throw ValidationException("fullName and email are required.");
    }
    if (!request.department.empty() && !Department::isValid(request.department)) {
        throw ValidationException("Invalid department value.");
    }
    if (!request.designation.empty() && !Designation::isValid(request.designation)) {
        throw ValidationException("Invalid designation value.");
    }

    auto optionalResource = resourceRepository->findById(resourceId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(resourceId) + " not found.");
    }

    Resource updatedResource       = optionalResource.value();
    updatedResource.fullName       = request.fullName;
    updatedResource.email          = request.email;
    if (!request.department.empty())  updatedResource.department  = request.department;
    if (!request.designation.empty()) updatedResource.designation = request.designation;

    resourceRepository->update(updatedResource);
    return updatedResource;
}

std::vector<Allocation> ResourceService::deactivateResource(int userId) {
    auto optionalResource = resourceRepository->findById(userId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(userId) + " not found.");
    }
    if (!optionalResource->isActive) {
        throw ValidationException("Resource is already inactive.");
    }
    const std::vector<Allocation> endedAllocations = allocationRepository->findActiveByUserId(userId);
    allocationRepository->endAllByUser(userId);
    resourceRepository->setActiveStatus(userId, false);
    resourceRepository->setStatus(userId, ResourceStatus::BENCH);
    return endedAllocations;
}

void ResourceService::assignManager(int userId, const AssignManagerRequest& request) {
    auto optionalResource = resourceRepository->findById(userId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(userId) + " not found.");
    }

    if (userId == request.managerId) {
        throw ValidationException("An resource cannot be assigned as their own manager.");
    }

    auto optionalManager = userRepository->findById(request.managerId);
    if (!optionalManager.has_value() || optionalManager->role != "MANAGER") {
        throw ValidationException("The specified user does not have the MANAGER role.");
    }

    resourceRepository->assignManager(userId, request.managerId);
}

std::vector<ResourceSkill> ResourceService::getSkills(int userId) {
    auto optionalResource = resourceRepository->findById(userId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(userId) + " not found.");
    }
    return skillRepository->findByUserId(userId);
}

ResourceSkill ResourceService::addSkill(int userId, const SkillRequest& request) {
    auto optionalResource = resourceRepository->findById(userId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("Resource with ID " + std::to_string(userId) + " not found.");
    }

    validateSkillFields(request);

    const auto existingSkills = skillRepository->findByUserId(userId);
    for (const auto& existingSkill : existingSkills) {
        if (existingSkill.skillName == request.skillName) {
            throw ValidationException(
                "'" + request.skillName + "' is already added. "
                "Use Update Skill to change the proficiency level."
            );
        }
    }

    ResourceSkill skill;
    skill.userId      = userId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    const int newSkillId = skillRepository->create(skill);
    skill.skillId = newSkillId;
    return skill;
}

void ResourceService::updateSkill(int userId, int skillId, const SkillRequest& request) {
    if (!skillRepository->skillBelongsToUser(skillId, userId)) {
        throw NotFoundException("Skill not found for this resource.");
    }

    validateSkillFields(request);

    ResourceSkill skill;
    skill.skillId     = skillId;
    skill.userId      = userId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    skillRepository->update(skill);
}

void ResourceService::removeSkill(int userId, int skillId) {
    if (!skillRepository->skillBelongsToUser(skillId, userId)) {
        throw NotFoundException("Skill not found for this resource.");
    }
    skillRepository->remove(skillId);
}

Resource ResourceService::changeRole(int userId, const ChangeRoleRequest& request, int requesterId) {

    const std::vector<std::string> VALID_ROLES = {"ADMIN", "MANAGER", "RESOURCE"};
    bool roleIsValid = std::any_of(VALID_ROLES.begin(), VALID_ROLES.end(),
                                   [&](const std::string& role){ return role == request.newRole; });
    if (!roleIsValid) {
        throw ValidationException("Invalid role. Must be ADMIN, MANAGER, or RESOURCE.");
    }


    if (userId == requesterId) {
        throw ValidationException("You cannot change your own role.");
    }

    auto optionalResource = resourceRepository->findById(userId);
    if (!optionalResource.has_value()) {
        throw NotFoundException("User with ID " + std::to_string(userId) + " not found.");
    }
    const Resource& current = optionalResource.value();

    if (current.role == request.newRole) {
        throw ValidationException("User already has the " + request.newRole + " role.");
    }


    if (current.role == "ADMIN") {
        const int activeAdmins = userRepository->countActiveAdmins();
        if (activeAdmins <= 1) {
            throw ValidationException(
                "Cannot change role: this is the only active admin account. "
                "Promote another user to ADMIN first.");
        }
    }


    if (current.role == "MANAGER") {
        const int projectCount = resourceRepository->countManagedProjects(userId);
        if (projectCount > 0) {
            throw ValidationException(
                "Cannot change role: user is still assigned as manager on " +
                std::to_string(projectCount) + " project(s). Reassign those projects first.");
        }
    }


    if (current.role == "RESOURCE") {
        allocationRepository->endAllByUser(userId);
        resourceRepository->deleteResourceStatus(userId);
    }


    if (request.newRole == "RESOURCE") {
        resourceRepository->createResourceStatus(userId);
    }


    std::string newDept;
    std::string newDesig;

    if (request.newRole == "ADMIN") {
        newDept  = Department::ADMINISTRATION;
        newDesig = Designation::ADMINISTRATOR;
    } else if (request.newRole == "MANAGER") {
        if (request.designation.empty()) {
            throw ValidationException("designation is required when changing role to MANAGER.");
        }
        if (!Designation::isValid(request.designation)) {
            throw ValidationException("Invalid designation value.");
        }
        newDept  = Department::MANAGEMENT;
        newDesig = request.designation;
    } else {
        if (request.department.empty() || request.designation.empty()) {
            throw ValidationException("department and designation are required when changing role to RESOURCE.");
        }
        if (!Department::isValid(request.department)) {
            throw ValidationException("Invalid department value.");
        }
        if (!Designation::isValid(request.designation)) {
            throw ValidationException("Invalid designation value.");
        }
        newDept  = request.department;
        newDesig = request.designation;
    }


    Resource updatedProfile = current;
    updatedProfile.department  = newDept;
    updatedProfile.designation = newDesig;
    resourceRepository->update(updatedProfile);


    userRepository->updateRole(userId, request.newRole);

    return resourceRepository->findById(userId).value();
}

