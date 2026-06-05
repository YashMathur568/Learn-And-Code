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
    if (request.fullName.empty() || request.email.empty() ||
        request.department.empty() || request.designation.empty()) {
        throw ValidationException("fullName, email, department, and designation are required.");
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

std::vector<Allocation> EmployeeService::deactivateEmployee(int employeeId) {
    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }
    if (!optionalEmployee->isActive) {
        throw ValidationException("Employee is already inactive.");
    }
    const std::vector<Allocation> endedAllocations = allocationRepository->findActiveByEmployeeId(employeeId);
    allocationRepository->endAllByEmployee(employeeId);
    employeeRepository->setActiveStatus(employeeId, false);
    employeeRepository->setStatus(employeeId, "BENCH");
    userRepository->setActiveStatus(optionalEmployee->userId, false);
    return endedAllocations;
}

void EmployeeService::assignManager(int employeeId, const AssignManagerRequest& request) {
    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }

    if (employeeId == request.managerEmployeeId) {
        throw ValidationException("An employee cannot be assigned as their own manager.");
    }

    auto optionalManager = employeeRepository->findById(request.managerEmployeeId);
    if (!optionalManager.has_value()) {
        throw NotFoundException("Manager employee with ID " + std::to_string(request.managerEmployeeId) + " not found.");
    }

    auto optionalManagerUser = userRepository->findById(optionalManager->userId);
    if (!optionalManagerUser.has_value() || optionalManagerUser->role != "MANAGER") {
        throw ValidationException("The specified employee does not have the MANAGER role.");
    }

    employeeRepository->assignManager(employeeId, request.managerEmployeeId);
}

std::vector<EmployeeSkill> EmployeeService::getSkills(int employeeId) {
    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }
    return skillRepository->findByEmployeeId(employeeId);
}

EmployeeSkill EmployeeService::addSkill(int employeeId, const SkillRequest& request) {
    auto optionalEmployee = employeeRepository->findById(employeeId);
    if (!optionalEmployee.has_value()) {
        throw NotFoundException("Employee with ID " + std::to_string(employeeId) + " not found.");
    }

    validateSkillFields(request);

    EmployeeSkill skill;
    skill.employeeId  = employeeId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    const int newSkillId = skillRepository->create(skill);
    skill.skillId = newSkillId;
    return skill;
}

void EmployeeService::updateSkill(int employeeId, int skillId, const SkillRequest& request) {
    if (!skillRepository->skillBelongsToEmployee(skillId, employeeId)) {
        throw NotFoundException("Skill not found for this employee.");
    }

    validateSkillFields(request);

    EmployeeSkill skill;
    skill.skillId     = skillId;
    skill.employeeId  = employeeId;
    skill.skillName   = request.skillName;
    skill.category    = request.category;
    skill.proficiency = request.proficiency;

    skillRepository->update(skill);
}

void EmployeeService::removeSkill(int employeeId, int skillId) {
    if (!skillRepository->skillBelongsToEmployee(skillId, employeeId)) {
        throw NotFoundException("Skill not found for this employee.");
    }
    skillRepository->remove(skillId);
}
