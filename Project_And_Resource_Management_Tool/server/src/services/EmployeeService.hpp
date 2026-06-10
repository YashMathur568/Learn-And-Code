#pragma once

#include "IEmployeeService.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/ISkillRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"

#include <memory>
#include <vector>

class EmployeeService : public IEmployeeService {
public:
    EmployeeService(
        std::shared_ptr<IEmployeeRepository>  employeeRepository,
        std::shared_ptr<IUserRepository>      userRepository,
        std::shared_ptr<ISkillRepository>     skillRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository
    );

    std::vector<Employee>    getAllEmployees()                                                     override;
    std::vector<Employee>    getByManagerId(int managerId)                                        override;
    Employee                 getById(int employeeId)                                              override;
    Employee                 updateEmployee(int employeeId, const UpdateEmployeeRequest& request) override;
    std::vector<Allocation>  deactivateEmployee(int employeeId)                                   override;
    void                     assignManager(int employeeId, const AssignManagerRequest& request)   override;

    std::vector<EmployeeSkill> getSkills(int employeeId)                                          override;
    EmployeeSkill              addSkill(int employeeId, const SkillRequest& request)              override;
    void                       updateSkill(int employeeId, int skillId,
                                           const SkillRequest& request)                           override;
    void                       removeSkill(int employeeId, int skillId)                           override;

private:
    std::shared_ptr<IEmployeeRepository>  employeeRepository;
    std::shared_ptr<IUserRepository>      userRepository;
    std::shared_ptr<ISkillRepository>     skillRepository;
    std::shared_ptr<IAllocationRepository> allocationRepository;

    static const std::vector<std::string> VALID_CATEGORIES;
    static const std::vector<std::string> VALID_PROFICIENCIES;

    void validateSkillFields(const SkillRequest& request) const;
};
