#include "EmployeeApiController.hpp"
#include "../services/AllocationService.hpp"
#include "../services/TimesheetService.hpp"
#include "../repositories/MySQLAllocationRepository.hpp"
#include "../repositories/MySQLProjectRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../repositories/MySQLTimesheetRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"
#include "../dto/AllocationDtos.hpp"

#include <nlohmann/json.hpp>

EmployeeApiController::EmployeeApiController()
    : allocationService(std::make_shared<AllocationService>(
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLEmployeeRepository>(),
          std::make_shared<MySQLProjectRepository>()
      )),
      timesheetService(std::make_shared<TimesheetService>(
          std::make_shared<MySQLTimesheetRepository>(),
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLEmployeeRepository>()
      )),
      projectRepository(std::make_shared<MySQLProjectRepository>()) {}

void EmployeeApiController::getMyAllocations(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireAnyRole(request, {"RESOURCE", "MANAGER"});
        const auto claims = RoleGuard::extractClaims(request);

        const auto allocations = allocationService->getActiveByUserId(claims.userId);

        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& allocation : allocations) {
            auto allocJson = allocationToJson(allocation);
            const auto project = projectRepository->findById(allocation.projectId);
            allocJson["projectName"] = project.has_value() ? project->name : "";
            dataArray.push_back(allocJson);
        }

        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void EmployeeApiController::getMyTimesheets(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireAnyRole(request, {"RESOURCE", "MANAGER"});
        const auto claims = RoleGuard::extractClaims(request);

        const auto timesheets = timesheetService->getByUserId(claims.userId);

        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& record : timesheets) {
            nlohmann::json tsJson = timesheetToJson(record.timesheet);

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

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void EmployeeApiController::submitTimesheet(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireAnyRole(request, {"RESOURCE", "MANAGER"});
        const auto claims = RoleGuard::extractClaims(request);

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto sheetRequest  = SubmitTimesheetRequest::fromJson(jsonBody);

        const TimesheetWithEntries result = timesheetService->submitTimesheet(claims.userId, sheetRequest);

        nlohmann::json tsJson = timesheetToJson(result.timesheet);
        nlohmann::json entriesArray = nlohmann::json::array();
        for (const auto& entry : result.entries) {
            entriesArray.push_back(entryToJson(entry));
        }
        tsJson["entries"] = entriesArray;

        callback(ResponseBuilder::success(tsJson, drogon::k201Created));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ConflictException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k409Conflict));
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
