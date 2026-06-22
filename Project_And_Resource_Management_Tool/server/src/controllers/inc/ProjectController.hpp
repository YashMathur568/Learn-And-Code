#pragma once

#include "IProjectService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class ProjectController : public drogon::HttpController<ProjectController> {
public:
    ProjectController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ProjectController::getAllProjects,   "/api/admin/projects",                              drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ProjectController::createProject,   "/api/admin/projects",                              drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(ProjectController::updateProject,   "/api/admin/projects/{id}",                         drogon::Put,  "JwtMiddleware");
        ADD_METHOD_TO(ProjectController::getMilestones,   "/api/admin/projects/{id}/milestones",              drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ProjectController::createMilestone, "/api/admin/projects/{id}/milestones",              drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(ProjectController::updateMilestone, "/api/admin/projects/{id}/milestones/{milestoneId}",drogon::Put,  "JwtMiddleware");
    METHOD_LIST_END

    void getAllProjects(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void createProject(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void updateProject(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void getMilestones(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void createMilestone(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void updateMilestone(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id,
        int milestoneId
    );

private:
    std::shared_ptr<IProjectService> projectService;
};
