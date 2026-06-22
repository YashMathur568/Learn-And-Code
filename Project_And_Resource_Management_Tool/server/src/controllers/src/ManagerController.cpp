#include "ManagerController.hpp"
#include "AllocationService.hpp"
#include "ProjectService.hpp"
#include "ResourceService.hpp"
#include "TimesheetService.hpp"
#include "MySQLAllocationRepository.hpp"
#include "MySQLProjectRepository.hpp"
#include "MySQLMilestoneRepository.hpp"
#include "MySQLResourceRepository.hpp"
#include "MySQLUserRepository.hpp"
#include "MySQLSkillRepository.hpp"
#include "MySQLTimesheetRepository.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"
#include "AdminDtos.hpp"
#include "ProjectDtos.hpp"
#include "AllocationDtos.hpp"
#include "ResourceStatus.hpp"
#include "ProjectHealth.hpp"

#include <nlohmann/json.hpp>
#include <ctime>

ManagerController::ManagerController()
    : allocationService(std::make_shared<AllocationService>(
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLResourceRepository>(),
          std::make_shared<MySQLProjectRepository>()
      )),
      projectService(std::make_shared<ProjectService>(
          std::make_shared<MySQLProjectRepository>(),
          std::make_shared<MySQLMilestoneRepository>(),
          std::make_shared<MySQLResourceRepository>()
      )),
      resourceService(std::make_shared<ResourceService>(
          std::make_shared<MySQLResourceRepository>(),
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLSkillRepository>(),
          std::make_shared<MySQLAllocationRepository>()
      )),
      timesheetService(std::make_shared<TimesheetService>(
          std::make_shared<MySQLTimesheetRepository>(),
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLResourceRepository>()
      )) {}

void ManagerController::getDashboard(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "ALLOCATE_RESOURCES");
        const auto claims = RoleGuard::extractClaims(request);

        const auto resources = resourceService->getByManagerId(claims.userId);

        nlohmann::json benchArray  = nlohmann::json::array();
        nlohmann::json activeArray = nlohmann::json::array();

        for (const auto& resource : resources) {
            const auto skills = resourceService->getSkills(resource.userId);
            nlohmann::json skillArray = nlohmann::json::array();
            for (const auto& skill : skills) {
                skillArray.push_back({
                    {"skillName",   skill.skillName},
                    {"category",    skill.category},
                    {"proficiency", skill.proficiency}
                });
            }

            nlohmann::json resourceJson = {
                {"userId",      resource.userId},
                {"fullName",    resource.fullName},
                {"department",  resource.department},
                {"designation", resource.designation}
            };

            if (resource.status == ResourceStatus::BENCH || resource.status.empty()) {
                const int totalUtil = allocationService->getTotalUtilisation(resource.userId);
                if (totalUtil == 0) {
                    resourceJson["skills"] = skillArray;
                    benchArray.push_back(resourceJson);
                } else {
                    resourceJson["totalAllocationPct"] = totalUtil;
                    activeArray.push_back(resourceJson);
                }
            } else {
                const int totalUtil = allocationService->getTotalUtilisation(resource.userId);
                resourceJson["totalAllocationPct"] = totalUtil;
                activeArray.push_back(resourceJson);
            }
        }

        callback(ResponseBuilder::success({
            {"benchResources",     benchArray},
            {"allocatedResources", activeArray}
        }));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getResourceDetail(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        PermissionGuard::require(request, "ALLOCATE_RESOURCES");
        const auto claims = RoleGuard::extractClaims(request);

        const auto resource = resourceService->getById(id);
        if (resource.managerId != claims.userId) {
            throw UnauthorizedException("This resource is not in your team.");
        }

        const auto allocations = allocationService->getActiveByUserId(id);
        const auto skills      = resourceService->getSkills(id);

        nlohmann::json allocationArray = nlohmann::json::array();
        for (const auto& allocation : allocations) {
            allocationArray.push_back(allocationToJson(allocation));
        }

        nlohmann::json skillArray = nlohmann::json::array();
        for (const auto& skill : skills) {
            skillArray.push_back(skillToJson(skill));
        }

        callback(ResponseBuilder::success({
            {"resource",    resourceToJson(resource)},
            {"allocations", allocationArray},
            {"skills",      skillArray}
        }));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::createAllocation(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "ALLOCATE_RESOURCES");
        const auto claims = RoleGuard::extractClaims(request);

        const auto jsonBody       = nlohmann::json::parse(request->getBody());
        const auto allocRequest   = CreateAllocationRequest::fromJson(jsonBody);

        const Allocation newAllocation = allocationService->createAllocation(claims.userId, allocRequest);
        callback(ResponseBuilder::success(allocationToJson(newAllocation), drogon::k201Created));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::endAllocation(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        PermissionGuard::require(request, "ALLOCATE_RESOURCES");
        const auto claims = RoleGuard::extractClaims(request);

        const Allocation endedAllocation = allocationService->endAllocation(id, claims.userId);

        const auto resource = resourceService->getById(endedAllocation.userId);
        const auto project  = projectService->getProjectById(endedAllocation.projectId);

        callback(ResponseBuilder::success({
            {"message",      "Allocation ended successfully."},
            {"allocation",   allocationToJson(endedAllocation)},
            {"resourceName", resource.fullName},
            {"projectName",  project.name}
        }));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getProjects(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROJECTS");
        const auto claims = RoleGuard::extractClaims(request);

        const auto projects = projectService->getProjectsByManagerId(claims.userId);

        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& project : projects) {
            dataArray.push_back(projectToJson(project));
        }

        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getProjectDetail(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROJECTS");
        const auto claims = RoleGuard::extractClaims(request);

        const auto project = projectService->getProjectById(id);
        if (project.managerId != claims.userId) {
            throw UnauthorizedException("You do not manage this project.");
        }

        const auto milestones  = projectService->getMilestones(id);
        const auto allocations = allocationService->getActiveByProjectId(id);

        nlohmann::json milestoneArray = nlohmann::json::array();
        for (const auto& milestone : milestones) {
            milestoneArray.push_back(milestoneToJson(milestone));
        }

        nlohmann::json teamArray = nlohmann::json::array();
        for (const auto& allocation : allocations) {
            const auto resource = resourceService->getById(allocation.userId);
            teamArray.push_back({
                {"allocationId",         allocation.allocationId},
                {"userId",               allocation.userId},
                {"resourceName",         resource.fullName},
                {"allocationPercentage", allocation.utilisation},
                {"fromDate",             allocation.fromDate},
                {"toDate",               allocation.toDate},
                {"isActive",             true}
            });
        }

        nlohmann::json riskFlags = nlohmann::json::array();
        if (project.health != ProjectHealth::ON_TRACK) {
            riskFlags.push_back("Project health: " + project.health);
        }

        const std::time_t now = std::time(nullptr);
        std::tm nowTimeStructure = {};
        localtime_r(&now, &nowTimeStructure);
        char todayBuf[11];
        std::strftime(todayBuf, sizeof(todayBuf), "%Y-%m-%d", &nowTimeStructure);
        const std::string today(todayBuf);

        for (const auto& milestone : milestones) {
            if (milestone.status != "DONE" && milestone.dueDate < today) {
                riskFlags.push_back("Overdue milestone: " + milestone.title);
            }
        }

        callback(ResponseBuilder::success([&]() {
            nlohmann::json dataObj = projectToJson(project);
            dataObj["milestones"]  = milestoneArray;
            dataObj["allocations"] = teamArray;
            dataObj["riskFlags"]   = riskFlags;
            return dataObj;
        }()));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getTeamTimesheets(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "VIEW_TEAM_TIMESHEETS");
        const auto claims = RoleGuard::extractClaims(request);

        const std::string weekStart = request->getParameter("weekStart");
        if (weekStart.empty()) {
            throw ValidationException("Query parameter 'weekStart' is required.");
        }

        const auto timesheets = timesheetService->getTeamTimesheets(claims.userId, weekStart);

        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& record : timesheets) {
            nlohmann::json tsJson = timesheetToJson(record.timesheet);

            const auto resource        = resourceService->getById(record.timesheet.userId);
            tsJson["resourceName"]     = resource.fullName;

            nlohmann::json entriesArray = nlohmann::json::array();
            int totalHours = 0;
            for (const auto& entry : record.entries) {
                entriesArray.push_back(entryToJson(entry));
                totalHours += entry.hours;
            }
            tsJson["entries"]    = entriesArray;
            tsJson["totalHours"] = totalHours;
            dataArray.push_back(tsJson);
        }

        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}
