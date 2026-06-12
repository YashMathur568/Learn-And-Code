#include "EmployeeService.hpp"
#include "../repositories/MySQLAllocationRepository.hpp"
#include "../utils/AppException.hpp"

#include <algorithm>

const std::vector<std::string> EmployeeService::VALID_CATEGORIES   = {"Backend", "Frontend", "DevOps", "QA", "Other"};
const std::vector<std::string> EmployeeService::VALID_PROFICIENCIES = {"Beginner", "Intermediate", "Advanced"};

EmployeeService::EmployeeService(
    std::shared_ptr<IEmployeeRepository>  employeeRepository,
    std::shared_ptr<IUserRepository>      userRepository,
    std::shared_ptr<ISkillRepository>     skillRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository
)
    : employeeRepository(std::move(employeeRepository))
    , userRepository(std::move(userRepository))
    , skillRepository(std::move(skillRepository))
    , allocationRepository(std::move(allocationRepository)) {}

void EmployeeService::validateSkillFields(const SkillRequest& request) const {
    if (request.skillName.empty()) {
        throw ValidationException("skillName is required.");
    }

    bool categoryValid = std::any_of(
        VALID_CATEGORIES.begin(), VALID_CATEGORIES.end(),
        [&request](const std::string& cat) { return cat == request.category; }
    );
    if (!categoryValid) {
        throw ValidationException("category must be one of: Backend, Frontend, DevOps, QA, Other.");
    }

    bool proficiencyValid = std::any_of(
        VALID_PROFICIENCIES.begin(), VALID_PROFICIENCIES.end(),
        [&request](const std::string& prof) { return prof == request.proficiency; }
    );
    if (!proficiencyValid) {
        throw ValidationException("proficiency must be one of: Beginner, Intermediate, Advanced.");
    }
}

std::vector<Employee> EmployeeService::getAllEmployees() {
    return employeeRepository->findAll();
}

std::vector<Employee> EmployeeService::getByManagerId(int managerId) {
    return employeeRepository->findByManagerId(managerId);
}

Employee EmployeeService::getById(int employeeId) {
    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }
    return optionalEmployee.value();
}

Employee EmployeeService::updateEmployee(int employeeId, const UpdateEmployeeRequest& request) {
    if (request.fullName.empty() || request.email.empty()) {
        throw ValidationException("fullName and email are required.");
    }

    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }

    Employee updatedEmployee       = optionalEmployee.value();
    updatedEmployee.fullName       = request.fullName;
    updatedEmployee.email          = request.email;
    updatedEmployee.department     = request.department;
    updatedEmployee.designation    = request.designation;

    employeeRepository->update(updatedEmployee);
    return updatedEmployee;
}

std::vector<Allocation> EmployeeService::deactivateEmployee(int userId) {
    auto optionalEmployee = employeeRepository->findById(userId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(userId) + " not found.");
    }
    if (!optionalEmployee->isActive) {
        throw ValidationException("Employee is already inactive.");
    }
    const std::vector<Allocation> endedAllocations = allocationRepository->findActiveByUserId(userId);
    allocationRepository->endAllByUser(userId);
    employeeRepository->setActiveStatus(userId, false);
    employeeRepository->setStatus(userId, "BENCH");
    return endedAllocations;
}

void EmployeeService::assignManager(int userId, const AssignManagerRequest& request) {
    auto optionalEmployee = employeeRepository->findById(userId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(userId) + " not found.");
    }

    if (userId == request.managerId) {
        throw ValidationException("An employee cannot be assigned as their own manager.");
    }

    auto optionalManager = userRepository->findById(request.managerId);
    if (!optionalManager.has_value() || optionalManager->role != "MANAGER") {
        throw ValidationException("The specified user does not have the MANAGER role.");
    }

    employeeRepository->assignManager(userId, request.managerId);
}

std::vector<EmployeeSkill> EmployeeService::getSkills(int userId) {
    auto optionalEmployee = employeeRepository->findById(userId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(userId) + " not found.");
    }
    return skillRepository->findByUserId(userId);
}

EmployeeSkill EmployeeService::addSkill(int userId, const SkillRequest& request) {
    auto optionalEmployee = employeeRepository->findById(userId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(userId) + " not found.");
    }

    validateSkillFields(request);

    EmployeeSkill skill;
    skill.userId      = userId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    const int newSkillId = skillRepository->create(skill);
    skill.skillId = newSkillId;
    return skill;
}

void EmployeeService::updateSkill(int userId, int skillId, const SkillRequest& request) {
    if (!skillRepository->skillBelongsToUser(skillId, userId)) {
        throw NotFoundException("Skill not found for this employee.");
    }

    validateSkillFields(request);

    EmployeeSkill skill;
    skill.skillId     = skillId;
    skill.userId      = userId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    skillRepository->update(skill);
}

void EmployeeService::removeSkill(int userId, int skillId) {
    if (!skillRepository->skillBelongsToUser(skillId, userId)) {
        throw NotFoundException("Skill not found for this employee.");
    }
    skillRepository->remove(skillId);
}

