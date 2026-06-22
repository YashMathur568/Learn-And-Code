#pragma once

#include "IProjectRepository.hpp"
#include "DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLProjectRepository : public IProjectRepository {
public:
    MySQLProjectRepository() = default;

    std::optional<Project> findById(int projectId)                 override;
    std::vector<Project>   findAll()                               override;
    std::vector<Project>   findByManagerId(int managerId)          override;
    int                    create(const Project& project)          override;
    void                   update(const Project& project)          override;
    void                   updateHealth(int projectId,
                                        const std::string& health) override;

private:
    static Project mapRowToProject(sql::ResultSet* resultSet);
};
