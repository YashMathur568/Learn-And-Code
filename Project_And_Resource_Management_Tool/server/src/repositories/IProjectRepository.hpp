#pragma once

#include "../models/Project.hpp"
#include <optional>
#include <string>
#include <vector>

class IProjectRepository {
public:
    virtual ~IProjectRepository() = default;

    virtual std::optional<Project> findById(int projectId)                     = 0;
    virtual std::vector<Project>   findAll()                                   = 0;
    virtual std::vector<Project>   findByManagerId(int managerId)              = 0;
    virtual int                    create(const Project& project)              = 0;
    virtual void                   update(const Project& project)              = 0;
    virtual void                   updateHealth(int projectId,
                                                const std::string& health)     = 0;
};
