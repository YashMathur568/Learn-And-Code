#pragma once

#include "IResourceService.hpp"
#include "IAllocationRepository.hpp"
#include <drogon/HttpController.h>
#include <memory>

class ResourceController : public drogon::HttpController<ResourceController> {
public:
    ResourceController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ResourceController::getAllResources,    "/api/admin/resources",                          drogon::Get,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::updateResource,    "/api/admin/resources/{id}",                     drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::deactivateResource,"/api/admin/resources/{id}/deactivate",          drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::assignManager,     "/api/admin/resources/{id}/assign-manager",      drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::getSkills,         "/api/admin/resources/{id}/skills",              drogon::Get,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::addSkill,          "/api/admin/resources/{id}/skills",              drogon::Post,   "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::updateSkill,       "/api/admin/resources/{id}/skills/{skillId}",    drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::removeSkill,       "/api/admin/resources/{id}/skills/{skillId}",    drogon::Delete, "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::restoreAccess,     "/api/admin/resources/{id}/restore-access",       drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(ResourceController::changeRole,        "/api/admin/resources/{id}/change-role",           drogon::Put,    "JwtMiddleware");
    METHOD_LIST_END

    void getAllResources(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void updateResource(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void deactivateResource(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void assignManager(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void getSkills(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void addSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void updateSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id,
        int skillId
    );

    void removeSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id,
        int skillId
    );

    void restoreAccess(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void changeRole(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

private:
    std::shared_ptr<IResourceService>    resourceService;
    std::shared_ptr<IAllocationRepository> allocationRepository;
};
