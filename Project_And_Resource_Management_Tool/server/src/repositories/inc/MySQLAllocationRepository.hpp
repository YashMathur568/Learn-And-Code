#pragma once

#include "IAllocationRepository.hpp"
#include "DatabasePool.hpp"
#include <cppconn/resultset.h>

class MySQLAllocationRepository : public IAllocationRepository {
public:
    std::optional<Allocation> findById(int allocationId)                       override;
    std::vector<Allocation>   findByUserId(int userId)                         override;
    std::vector<Allocation>   findActiveByUserId(int userId)                   override;
    std::vector<Allocation>   findActiveByProjectId(int projectId)             override;
    int                       getTotalActiveUtilisation(int userId)            override;
    bool                      wasAllocatedDuringWeek(int userId, int projectId, const std::string& weekStart) override;
    std::string               getProjectStatus(int projectId)                 override;
    int                       create(const Allocation& allocation)             override;
    void                      end(int allocationId)                            override;
    void                      endAllByUser(int userId)                         override;

private:
    static Allocation mapRowToAllocation(sql::ResultSet* resultSet);
};
