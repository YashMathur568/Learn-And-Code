#pragma once

#include "Resource.hpp"
#include <optional>
#include <string>
#include <vector>

class IResourceRepository {
public:
    virtual ~IResourceRepository() = default;

    virtual std::optional<Resource> findById(int userId)                         = 0;
    virtual std::vector<Resource>   findAll()                                    = 0;
    virtual std::vector<Resource>   findAllActive()                              = 0;
    virtual std::vector<Resource>   findByManagerId(int managerUserId)           = 0;
    virtual int                     create(const Resource& resource)             = 0;
    virtual void                    update(const Resource& resource)             = 0;
    virtual void                    setActiveStatus(int userId, bool active)     = 0;
    virtual void                    setStatus(int userId,
                                              const std::string& status)         = 0;
    virtual void                    assignManager(int userId,
                                                  int managerUserId)             = 0;
    virtual void                    setFrozen(int userId, bool frozen)            = 0;
    virtual bool                    isFrozen(int userId)                         = 0;
    virtual bool                    hasActiveAllocations(int userId)              = 0;
    virtual bool                    existsByUserId(int userId)                    = 0;
    virtual void                    deleteResourceStatus(int userId)              = 0;
    virtual void                    createResourceStatus(int userId)              = 0;
    virtual int                     countManagedProjects(int userId)              = 0;
};
