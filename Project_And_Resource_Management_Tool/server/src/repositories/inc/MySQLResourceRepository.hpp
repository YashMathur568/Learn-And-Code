#pragma once

#include "IResourceRepository.hpp"
#include "DatabasePool.hpp"

namespace sql {
    class ResultSet;
}

class MySQLResourceRepository : public IResourceRepository {
public:
    MySQLResourceRepository() = default;

    std::optional<Resource> findById(int userId)                         override;
    std::vector<Resource>   findAll()                                    override;
    std::vector<Resource>   findAllActive()                              override;
    std::vector<Resource>   findByManagerId(int managerUserId)           override;
    int                     create(const Resource& resource)             override;
    void                    update(const Resource& resource)             override;
    void                    setActiveStatus(int userId, bool active)     override;
    void                    setStatus(int userId,
                                      const std::string& status)         override;
    void                    assignManager(int userId, int managerUserId) override;
    void                    setFrozen(int userId, bool frozen)           override;
    bool                    isFrozen(int userId)                        override;
    bool                    hasActiveAllocations(int userId)             override;
    bool                    existsByUserId(int userId)                   override;
    void                    deleteResourceStatus(int userId)             override;
    void                    createResourceStatus(int userId)             override;
    int                     countManagedProjects(int userId)             override;

private:
    static Resource mapRowToResource(sql::ResultSet* resultSet);
};
