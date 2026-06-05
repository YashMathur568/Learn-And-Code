#include "ManagerController.hpp"
#include "../repositories/MySQLAllocationRepository.hpp"
#include "../repositories/MySQLProjectRepository.hpp"
#include "../repositories/MySQLMilestoneRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../repositories/MySQLUserRepository.hpp"
#include "../repositories/MySQLSkillRepository.hpp"
#include "../repositories/MySQLTimesheetRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"
#include "../dto/AdminDtos.hpp"
#include "../dto/ProjectDtos.hpp"
#include "../dto/AllocationDtos.hpp"

#include <nlohmann/json.hpp>
#include <ctime>

ManagerController::ManagerController()
    : allocationService(std::make_shared<AllocationService>(
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLEmployeeRepository>(),
          std::make_shared<MySQLProjectRepository>()
      )),
      projectService(std::make_shared<ProjectService>(
          std::make_shared<MySQLProjectRepository>(),
          std::make_shared<MySQLMilestoneRepository>()
      )),
      employeeService(std::make_shared<EmployeeService>(
          std::make_shared<MySQLEmployeeRepository>(),
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLSkillRepository>(),
          std::make_shared<MySQLAllocationRepository>()
      )),
      timesheetService(std::make_shared<TimesheetService>(
          std::make_shared<MySQLTimesheetRepository>(),
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLEmployeeRepository>()
      )) {}

void ManagerController::getDashboard(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const auto employees = employeeService->getByManagerId(claims.employeeId);

        nlohmann::json benchArray  = nlohmann::json::array();
        nlohmann::json activeArray = nlohmann::json::array();

        for (const auto& employee : employees) {
            nlohmann::json empJson = {
                {"employeeId",  employee.employeeId},
                {"fullName",    employee.fullName},
                {"department",  employee.department},
                {"designation", employee.designation}
            };

            if (employee.status == "BENCH") {
                benchArray.push_back(empJson);
            } else {
                const int totalUtilisation = allocationService->getTotalUtilisation(employee.employeeId);
                empJson["totalUtilisation"] = totalUtilisation;
                activeArray.push_back(empJson);
            }
        }

        callback(ResponseBuilder::success({
            {"bench",  benchArray},
            {"active", activeArray}
        }));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getEmployeeDetail(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const auto employee = employeeService->getById(id);
        if (employee.managerId != claims.employeeId) {
            throw UnauthorizedException("This employee is not in your team.");
        }

        const auto allocations = allocationService->getActiveByEmployeeId(id);
        const auto skills      = employeeService->getSkills(id);

        nlohmann::json allocArray = nlohmann::json::array();
        for (const auto& alloc : allocations) {
            allocArray.push_back(allocationToJson(alloc));
        }

        nlohmann::json skillArray = nlohmann::json::array();
        for (const auto& skill : skills) {
            skillArray.push_back(skillToJson(skill));
        }

        callback(ResponseBuilder::success({
            {"employee",    employeeToJson(employee)},
            {"allocations", allocArray},
            {"skills",      skillArray}
        }));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::createAllocation(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const auto jsonBody       = nlohmann::json::parse(request->getBody());
        const auto allocRequest   = CreateAllocationRequest::fromJson(jsonBody);

        const Allocation newAllocation = allocationService->createAllocation(claims.employeeId, allocRequest);
        callback(ResponseBuilder::success(allocationToJson(newAllocation), drogon::k201Created));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::endAllocation(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const Allocation endedAllocation = allocationService->endAllocation(id, claims.employeeId);

        const auto employee = employeeService->getById(endedAllocation.employeeId);
        const auto project  = projectService->getProjectById(endedAllocation.projectId);

        callback(ResponseBuilder::success({
            {"message",      "Allocation ended successfully."},
            {"allocation",   allocationToJson(endedAllocation)},
            {"employeeName", employee.fullName},
            {"projectName",  project.name}
        }));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getProjects(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const auto projects = projectService->getProjectsByManagerId(claims.employeeId);

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

void ManagerController::getProjectDetail(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const auto project = projectService->getProjectById(id);
        if (project.managerId != claims.employeeId) {
            throw UnauthorizedException("You do not manage this project.");
        }

        const auto milestones  = projectService->getMilestones(id);
        const auto allocations = allocationService->getActiveByProjectId(id);

        nlohmann::json milestoneArray = nlohmann::json::array();
        for (const auto& milestone : milestones) {
            milestoneArray.push_back(milestoneToJson(milestone));
        }

        nlohmann::json teamArray = nlohmann::json::array();
        for (const auto& alloc : allocations) {
            const auto employee = employeeService->getById(alloc.employeeId);
            teamArray.push_back({
                {"allocationId",   alloc.allocationId},
                {"employeeId",     alloc.employeeId},
                {"employeeName",   employee.fullName},
                {"utilisation",    alloc.utilisation},
                {"fromDate",       alloc.fromDate},
                {"toDate",         alloc.toDate}
            });
        }

        nlohmann::json riskFlags = nlohmann::json::array();
        if (project.health != "ON_TRACK") {
            riskFlags.push_back("Project health: " + project.health);
        }

        const std::time_t now = std::time(nullptr);
        std::tm nowTm = {};
        localtime_r(&now, &nowTm);
        char todayBuf[11];
        std::strftime(todayBuf, sizeof(todayBuf), "%Y-%m-%d", &nowTm);
        const std::string today(todayBuf);

        for (const auto& milestone : milestones) {
            if (milestone.status != "DONE" && milestone.dueDate < today) {
                riskFlags.push_back("Overdue milestone: " + milestone.title);
            }
        }

        callback(ResponseBuilder::success({
            {"project",    projectToJson(project)},
            {"milestones", milestoneArray},
            {"team",       teamArray},
            {"riskFlags",  riskFlags}
        }));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void ManagerController::getTeamTimesheets(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const std::string weekStart = request->getParameter("weekStart");
        if (weekStart.empty()) {
            throw ValidationException("Query parameter 'weekStart' is required.");
        }

        const auto timesheets = timesheetService->getTeamTimesheets(claims.employeeId, weekStart);

        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& record : timesheets) {
            nlohmann::json tsJson = timesheetToJson(record.timesheet);

            const auto employee        = employeeService->getById(record.timesheet.employeeId);
            tsJson["employeeName"]     = employee.fullName;

            nlohmann::json entriesArray = nlohmann::json::array();
            for (const auto& entry : record.entries) {
                entriesArray.push_back(entryToJson(entry));
            }
            tsJson["entries"] = entriesArray;
            dataArray.push_back(tsJson);
        }

        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
