#pragma once

#include "ISkillMatchService.hpp"
#include "IRiskSummaryService.hpp"
#include "TeamBuilderService.hpp"
#include "MySQLProjectRepository.hpp"
#include "MySQLMilestoneRepository.hpp"
#include "MySQLAllocationRepository.hpp"
#include "MySQLTimesheetRepository.hpp"
#include "MySQLResourceRepository.hpp"
#include <drogon/HttpController.h>
#include <memory>

class AIController : public drogon::HttpController<AIController> {
public:
    AIController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AIController::skillMatch,       "/api/manager/ai/skill-match",        drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::riskSummary,      "/api/manager/ai/risk-summary/{id}",  drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::buildTeam,        "/api/manager/ai/team-builder",       drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::getAiProvider,    "/api/manager/ai/provider",           drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(AIController::switchAiProvider, "/api/manager/ai/provider",           drogon::Put,  "JwtMiddleware");
    METHOD_LIST_END

    void skillMatch(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void riskSummary(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void buildTeam(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void getAiProvider(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void switchAiProvider(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

private:
    std::shared_ptr<MySQLProjectRepository>    projectRepo_;
    std::shared_ptr<MySQLMilestoneRepository>  milestoneRepo_;
    std::shared_ptr<MySQLAllocationRepository> allocationRepo_;
    std::shared_ptr<MySQLTimesheetRepository>  timesheetRepo_;
    std::shared_ptr<MySQLResourceRepository>   resourceRepo_;
};
