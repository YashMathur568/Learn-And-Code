#pragma once

#include "IEmployeeRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLEmployeeRepository : public IEmployeeRepository {
public:
    MySQLEmployeeRepository() = default;

    std::optional<Employee> findById(int employeeId)                     override;
    std::optional<Employee> findByUserId(int userId)                     override;
    std::vector<Employee>   findAll()                                    override;
    std::vector<Employee>   findAllActive()                              override;
    std::vector<Employee>   findByManagerId(int managerEmployeeId)       override;
    int                     create(const Employee& employee)             override;
    void                    update(const Employee& employee)             override;
    void                    setActiveStatus(int employeeId, bool active) override;
    void                    setStatus(int employeeId,
                                      const std::string& status)         override;
    void                    assignManager(int employeeId,
                                          int managerEmployeeId)         override;
    bool                    hasActiveAllocations(int employeeId)         override;
    bool                    existsByUserId(int userId)                   override;

private:
    static Employee mapRowToEmployee(sql::ResultSet* resultSet);
};
