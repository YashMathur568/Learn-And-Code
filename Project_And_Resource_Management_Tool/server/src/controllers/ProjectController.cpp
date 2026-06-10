#include "ProjectController.hpp"
#include "../services/ProjectService.hpp"
#include "../repositories/MySQLProjectRepository.hpp"
#include "../repositories/MySQLMilestoneRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

ProjectController::ProjectController()
    : projectService(std::make_shared<ProjectService>(
          std::make_shared<MySQLProjectRepository>(),
          std::make_shared<MySQLMilestoneRepository>(),
          std::make_shared<MySQLEmployeeRepository>()
      )) {}

void ProjectController::getAllProjects(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireAnyRole(request, {"ADMIN", "MANAGER"});

        const auto projects = projectService->getAllProjects();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& project : projects) {
            dataArray.push_back(projectToJson(project));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ProjectController::createProject(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody       = nlohmann::json::parse(request->getBody());
        const auto createRequest  = CreateProjectRequest::fromJson(jsonBody);

        const Project newProject = projectService->createProject(createRequest);
        callback(ResponseBuilder::success(projectToJson(newProject), drogon::k201Created));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ProjectController::updateProject(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int projectId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto updateRequest = UpdateProjectRequest::fromJson(jsonBody);

        const Project updatedProject = projectService->updateProject(projectId, updateRequest);
        callback(ResponseBuilder::success(projectToJson(updatedProject)));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ProjectController::getMilestones(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int projectId
) {
    try {
        RoleGuard::requireAnyRole(request, {"ADMIN", "MANAGER"});

        const auto milestones = projectService->getMilestones(projectId);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& milestone : milestones) {
            dataArray.push_back(milestoneToJson(milestone));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ProjectController::createMilestone(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int projectId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody       = nlohmann::json::parse(request->getBody());
        const auto createRequest  = CreateMilestoneRequest::fromJson(jsonBody);

        const auto milestones = projectService->createMilestone(projectId, createRequest);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& milestone : milestones) { dataArray.push_back(milestoneToJson(milestone)); }
        callback(ResponseBuilder::success({{"data", dataArray}}, drogon::k201Created));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ProjectController::updateMilestone(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int projectId,
    int milestoneId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto updateRequest = UpdateMilestoneRequest::fromJson(jsonBody);

        const auto milestones = projectService->updateMilestone(projectId, milestoneId, updateRequest);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& milestone : milestones) { dataArray.push_back(milestoneToJson(milestone)); }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
