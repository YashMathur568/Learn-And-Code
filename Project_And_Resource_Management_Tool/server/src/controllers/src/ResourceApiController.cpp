#include "ResourceApiController.hpp"
#include "AllocationService.hpp"
#include "TimesheetService.hpp"
#include "MySQLAllocationRepository.hpp"
#include "MySQLProjectRepository.hpp"
#include "MySQLResourceRepository.hpp"
#include "MySQLTimesheetRepository.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"
#include "AllocationDtos.hpp"

#include <nlohmann/json.hpp>

ResourceApiController::ResourceApiController()
    : allocationService(std::make_shared<AllocationService>(
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLResourceRepository>(),
          std::make_shared<MySQLProjectRepository>()
      )),
      timesheetService(std::make_shared<TimesheetService>(
          std::make_shared<MySQLTimesheetRepository>(),
          std::make_shared<MySQLAllocationRepository>(),
          std::make_shared<MySQLResourceRepository>()
      )),
      projectRepository(std::make_shared<MySQLProjectRepository>()),
      resourceRepository(std::make_shared<MySQLResourceRepository>()) {}

void ResourceApiController::getCurrentResource(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        const auto claims = RoleGuard::extractClaims(request);
        const auto resource = resourceRepository->findById(claims.userId);
        if (!resource.has_value()) {
            throw NotFoundException("Resource profile not found.");
        }
        callback(ResponseBuilder::success({
            {"userId",   resource->userId},
            {"fullName", resource->fullName},
            {"isFrozen", resource->isFrozen},
            {"status",   resource->status},
            {"role",     resource->role}
        }));
    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceApiController::getMyAllocations(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
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

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceApiController::getMyTimesheets(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
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

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceApiController::submitTimesheet(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "SUBMIT_TIMESHEET");
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

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ConflictException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k409Conflict));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}
