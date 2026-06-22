#include "ResourceController.hpp"
#include "ResourceService.hpp"
#include "MySQLResourceRepository.hpp"
#include "MySQLUserRepository.hpp"
#include "MySQLSkillRepository.hpp"
#include "MySQLAllocationRepository.hpp"
#include "AllocationDtos.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"
#include "ResourceStatus.hpp"

#include <nlohmann/json.hpp>

ResourceController::ResourceController()
    : resourceService(std::make_shared<ResourceService>(
          std::make_shared<MySQLResourceRepository>(),
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLSkillRepository>(),
          std::make_shared<MySQLAllocationRepository>()
      )),
      allocationRepository(std::make_shared<MySQLAllocationRepository>()) {}

void ResourceController::getAllResources(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::requireAny(request, {"MANAGE_PROFILES", "ALLOCATE_RESOURCES"});

        const auto resources = resourceService->getAllResources();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& resource : resources) {
            nlohmann::json resourceJson = resourceToJson(resource);
            if (resource.status == ResourceStatus::ALLOCATED) {
                const auto allocations = allocationRepository->findActiveByUserId(resource.userId);
                nlohmann::json allocationArray = nlohmann::json::array();
                for (const auto& allocation : allocations)
                    allocationArray.push_back(allocationToJson(allocation));
                resourceJson["allocations"] = allocationArray;
            } else {
                resourceJson["allocations"] = nlohmann::json::array();
            }
            dataArray.push_back(resourceJson);
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceController::updateResource(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROFILES");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto updateRequest = UpdateResourceRequest::fromJson(jsonBody);

        const Resource updatedResource = resourceService->updateResource(resourceId, updateRequest);
        callback(ResponseBuilder::success(resourceToJson(updatedResource)));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
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

void ResourceController::deactivateResource(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROFILES");

        const auto claims = RoleGuard::extractClaims(request);
        if (claims.userId == resourceId) {
            callback(ResponseBuilder::error("You cannot deactivate your own account.", drogon::k400BadRequest));
            return;
        }

        const std::vector<Allocation> endedAllocations = resourceService->deactivateResource(resourceId);

        nlohmann::json endedArray = nlohmann::json::array();
        for (const auto& allocation : endedAllocations) {
            endedArray.push_back(allocationToJson(allocation));
        }
        callback(ResponseBuilder::success({
            {"message",          "Resource deactivated successfully."},
            {"endedAllocations", endedArray}
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

void ResourceController::assignManager(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROFILES");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto assignRequest = AssignManagerRequest::fromJson(jsonBody);

        resourceService->assignManager(resourceId, assignRequest);
        callback(ResponseBuilder::success({{"message", "Manager assigned successfully."}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
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

void ResourceController::getSkills(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_SKILLS");

        const auto skills = resourceService->getSkills(resourceId);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& skill : skills) {
            dataArray.push_back(skillToJson(skill));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceController::addSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_SKILLS");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto skillRequest = SkillRequest::fromJson(jsonBody);

        const ResourceSkill newSkill = resourceService->addSkill(resourceId, skillRequest);
        callback(ResponseBuilder::success(skillToJson(newSkill), drogon::k201Created));

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

void ResourceController::updateSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId,
    int skillId
) {
    try {
        PermissionGuard::require(request, "MANAGE_SKILLS");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto skillRequest = SkillRequest::fromJson(jsonBody);

        resourceService->updateSkill(resourceId, skillId, skillRequest);

        const auto updatedSkills = resourceService->getSkills(resourceId);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& skill : updatedSkills) { dataArray.push_back(skillToJson(skill)); }
        callback(ResponseBuilder::success({{"data", dataArray}}));

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

void ResourceController::removeSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId,
    int skillId
) {
    try {
        PermissionGuard::require(request, "MANAGE_SKILLS");

        resourceService->removeSkill(resourceId, skillId);

        const auto remainingSkills = resourceService->getSkills(resourceId);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& skill : remainingSkills) { dataArray.push_back(skillToJson(skill)); }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceController::restoreAccess(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::requireAny(request, {"MANAGE_PROFILES", "VIEW_TEAM_TIMESHEETS"});
        const auto claims = RoleGuard::extractClaims(request);

        auto resRepo = std::make_shared<MySQLResourceRepository>();
        resRepo->setFrozen(resourceId, false);

        callback(ResponseBuilder::success({{"message", "Access restored successfully."}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void ResourceController::changeRole(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int resourceId
) {
    try {
        PermissionGuard::require(request, "MANAGE_PROFILES");

        const auto claims      = RoleGuard::extractClaims(request);
        const auto jsonBody    = nlohmann::json::parse(request->getBody());
        const auto roleRequest = ChangeRoleRequest::fromJson(jsonBody);

        const Resource updated = resourceService->changeRole(resourceId, roleRequest, claims.userId);
        callback(ResponseBuilder::success(resourceToJson(updated)));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
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
