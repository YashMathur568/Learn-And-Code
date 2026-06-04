#include "EmployeeController.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../repositories/MySQLUserRepository.hpp"
#include "../repositories/MySQLSkillRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

EmployeeController::EmployeeController()
    : employeeService(std::make_shared<EmployeeService>(
          std::make_shared<MySQLEmployeeRepository>(),
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLSkillRepository>()
      )) {}

void EmployeeController::getAllEmployees(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto employees = employeeService->getAllEmployees();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& employee : employees) {
            dataArray.push_back(employeeToJson(employee));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void EmployeeController::updateEmployee(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto updateRequest = UpdateEmployeeRequest::fromJson(jsonBody);

        employeeService->updateEmployee(employeeId, updateRequest);
        callback(ResponseBuilder::success({{"message", "Employee updated successfully."}}));

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

void EmployeeController::deactivateEmployee(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        employeeService->deactivateEmployee(employeeId);
        callback(ResponseBuilder::success({{"message", "Employee deactivated successfully."}}));

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

void EmployeeController::assignManager(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto assignRequest = AssignManagerRequest::fromJson(jsonBody);

        employeeService->assignManager(employeeId, assignRequest);
        callback(ResponseBuilder::success({{"message", "Manager assigned successfully."}}));

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

void EmployeeController::getSkills(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto skills = employeeService->getSkills(employeeId);
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& skill : skills) {
            dataArray.push_back(skillToJson(skill));
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

void EmployeeController::addSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto skillRequest = SkillRequest::fromJson(jsonBody);

        const int newSkillId = employeeService->addSkill(employeeId, skillRequest);
        callback(ResponseBuilder::success({{"skillId", newSkillId}}, drogon::k201Created));

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

void EmployeeController::updateSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId,
    int skillId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto skillRequest = SkillRequest::fromJson(jsonBody);

        employeeService->updateSkill(employeeId, skillId, skillRequest);
        callback(ResponseBuilder::success({{"message", "Skill updated successfully."}}));

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

void EmployeeController::removeSkill(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int employeeId,
    int skillId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        employeeService->removeSkill(employeeId, skillId);
        callback(ResponseBuilder::success({{"message", "Skill removed successfully."}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
