#pragma once

#include "../dto/AdminDtos.hpp"
#include "../models/Employee.hpp"
#include "../models/EmployeeSkill.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/ISkillRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"

#include <memory>
#include <vector>

class EmployeeService {
public:
    EmployeeService(
        std::shared_ptr<IEmployeeRepository>  employeeRepository,
        std::shared_ptr<IUserRepository>      userRepository,
        std::shared_ptr<ISkillRepository>     skillRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository
    );

    std::vector<Employee>    getAllEmployees();
    std::vector<Employee>    getByManagerId(int managerId);
    Employee                 getById(int employeeId);
    Employee                 updateEmployee(int employeeId, const UpdateEmployeeRequest& request);
    std::vector<Allocation>  deactivateEmployee(int employeeId);
    void                     assignManager(int employeeId, const AssignManagerRequest& request);

    std::vector<EmployeeSkill> getSkills(int employeeId);
    EmployeeSkill              addSkill(int employeeId, const SkillRequest& request);
    void                       updateSkill(int employeeId, int skillId, const SkillRequest& request);
    void                       removeSkill(int employeeId, int skillId);

private:
    std::shared_ptr<IEmployeeRepository>  employeeRepository;
    std::shared_ptr<IUserRepository>      userRepository;
    std::shared_ptr<ISkillRepository>     skillRepository;
    std::shared_ptr<IAllocationRepository> allocationRepository;

    static const std::vector<std::string> VALID_CATEGORIES;
    static const std::vector<std::string> VALID_PROFICIENCIES;

    void validateSkillFields(const SkillRequest& request) const;
};
