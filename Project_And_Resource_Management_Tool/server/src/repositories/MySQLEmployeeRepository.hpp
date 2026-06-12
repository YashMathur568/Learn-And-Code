#pragma once

#include "IEmployeeRepository.hpp"
#include "../utils/DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLEmployeeRepository : public IEmployeeRepository {
public:
    MySQLEmployeeRepository() = default;

    std::optional<Employee> findById(int userId)                         override;
    std::vector<Employee>   findAll()                                    override;
    std::vector<Employee>   findAllActive()                              override;
    std::vector<Employee>   findByManagerId(int managerUserId)           override;
    int                     create(const Employee& employee)             override;
    void                    update(const Employee& employee)             override;
    void                    setActiveStatus(int userId, bool active)     override;
    void                    setStatus(int userId,
                                      const std::string& status)         override;
    void                    assignManager(int userId, int managerUserId) override;
    bool                    hasActiveAllocations(int userId)             override;
    bool                    existsByUserId(int userId)                   override;

private:
    static Employee mapRowToEmployee(sql::ResultSet* resultSet);
};
