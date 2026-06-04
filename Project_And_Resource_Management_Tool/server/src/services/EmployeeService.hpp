#pragma once

#include "../dto/AdminDtos.hpp"
#include "../models/Employee.hpp"
#include "../models/EmployeeSkill.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/ISkillRepository.hpp"

#include <memory>
#include <vector>

class EmployeeService {
public:
    EmployeeService(
        std::shared_ptr<IEmployeeRepository> employeeRepository,
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<ISkillRepository>    skillRepository
    );

    std::vector<Employee>    getAllEmployees();
    void                     updateEmployee(int employeeId, const UpdateEmployeeRequest& request);
    void                     deactivateEmployee(int employeeId);
    void                     assignManager(int employeeId, const AssignManagerRequest& request);

    std::vector<EmployeeSkill> getSkills(int employeeId);
    int                        addSkill(int employeeId, const SkillRequest& request);
    void                       updateSkill(int employeeId, int skillId, const SkillRequest& request);
    void                       removeSkill(int employeeId, int skillId);

private:
    std::shared_ptr<IEmployeeRepository> employeeRepository;
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<ISkillRepository>    skillRepository;

    static const std::vector<std::string> VALID_CATEGORIES;
    static const std::vector<std::string> VALID_PROFICIENCIES;

    void validateSkillFields(const SkillRequest& request) const;
};
