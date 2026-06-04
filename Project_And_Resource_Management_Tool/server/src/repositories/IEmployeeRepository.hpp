#pragma once

#include "../models/Employee.hpp"
#include <optional>
#include <string>
#include <vector>

class IEmployeeRepository {
public:
    virtual ~IEmployeeRepository() = default;

    virtual std::optional<Employee> findById(int employeeId)                       = 0;
    virtual std::optional<Employee> findByUserId(int userId)                       = 0;
    virtual std::vector<Employee>   findAll()                                      = 0;
    virtual std::vector<Employee>   findAllActive()                                = 0;
    virtual std::vector<Employee>   findByManagerId(int managerEmployeeId)         = 0;
    virtual int                     create(const Employee& employee)               = 0;
    virtual void                    update(const Employee& employee)               = 0;
    virtual void                    setActiveStatus(int employeeId, bool active)   = 0;
    virtual void                    setStatus(int employeeId,
                                              const std::string& status)           = 0;
    virtual void                    assignManager(int employeeId,
                                                  int managerEmployeeId)           = 0;
    virtual bool                    hasActiveAllocations(int employeeId)           = 0;
    virtual bool                    existsByUserId(int userId)                     = 0;
};
