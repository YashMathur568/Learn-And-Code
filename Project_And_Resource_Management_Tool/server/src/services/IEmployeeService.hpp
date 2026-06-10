#pragma once

#include "../dto/AdminDtos.hpp"
#include "../models/Employee.hpp"
#include "../models/EmployeeSkill.hpp"
#include "../models/Allocation.hpp"

#include <vector>

class IEmployeeService {
public:
    virtual ~IEmployeeService() = default;

    virtual std::vector<Employee>    getAllEmployees()                                                     = 0;
    virtual std::vector<Employee>    getByManagerId(int managerId)                                        = 0;
    virtual Employee                 getById(int employeeId)                                              = 0;
    virtual Employee                 updateEmployee(int employeeId, const UpdateEmployeeRequest& request) = 0;
    virtual std::vector<Allocation>  deactivateEmployee(int employeeId)                                   = 0;
    virtual void                     assignManager(int employeeId, const AssignManagerRequest& request)   = 0;

    virtual std::vector<EmployeeSkill> getSkills(int employeeId)                                          = 0;
    virtual EmployeeSkill              addSkill(int employeeId, const SkillRequest& request)              = 0;
    virtual void                       updateSkill(int employeeId, int skillId,
                                                   const SkillRequest& request)                           = 0;
    virtual void                       removeSkill(int employeeId, int skillId)                           = 0;
};
