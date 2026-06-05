#pragma once

#include "IAllocationRepository.hpp"
#include "../utils/DatabasePool.hpp"
#include <cppconn/resultset.h>

class MySQLAllocationRepository : public IAllocationRepository {
public:
    std::optional<Allocation> findById(int allocationId)                         override;
    std::vector<Allocation>   findByEmployeeId(int employeeId)                   override;
    std::vector<Allocation>   findActiveByEmployeeId(int employeeId)             override;
    std::vector<Allocation>   findActiveByProjectId(int projectId)               override;
    int                       getTotalActiveUtilisation(int employeeId)          override;
    bool                      isActivelyAllocated(int employeeId, int projectId) override;
    int                       create(const Allocation& allocation)               override;
    void                      end(int allocationId)                              override;
    void                      endAllByEmployee(int employeeId)                    override;

private:
    static Allocation mapRowToAllocation(sql::ResultSet* resultSet);
};
