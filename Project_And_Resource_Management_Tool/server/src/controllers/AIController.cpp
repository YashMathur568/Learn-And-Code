#include "AIController.hpp"
#include "../ai/LLMAdapterFactory.hpp"
#include "../repositories/MySQLProjectRepository.hpp"
#include "../repositories/MySQLMilestoneRepository.hpp"
#include "../repositories/MySQLAllocationRepository.hpp"
#include "../repositories/MySQLTimesheetRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

AIController::AIController() {
    auto llmAdapter = LLMAdapterFactory::create();

    skillMatchService_ = std::make_shared<SkillMatchService>(llmAdapter);

    riskSummaryService_ = std::make_shared<RiskSummaryService>(
        llmAdapter,
        std::make_shared<MySQLProjectRepository>(),
        std::make_shared<MySQLMilestoneRepository>(),
        std::make_shared<MySQLAllocationRepository>(),
        std::make_shared<MySQLTimesheetRepository>(),
        std::make_shared<MySQLEmployeeRepository>()
    );
}

void AIController::skillMatch(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string query = jsonBody.at("query").get<std::string>();

        if (query.empty()) {
            callback(ResponseBuilder::error("query field is required.", drogon::k400BadRequest));
            return;
        }

        const auto results = skillMatchService_->findMatches(query);
        callback(ResponseBuilder::success({{"data", results}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void AIController::riskSummary(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        RoleGuard::requireRole(request, "MANAGER");
        const auto claims = RoleGuard::extractClaims(request);

        const std::string summary = riskSummaryService_->generateSummary(id, claims.employeeId);
        callback(ResponseBuilder::success({{"summary", summary}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
