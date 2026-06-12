#pragma once

#include "../services/ISkillMatchService.hpp"
#include "../services/IRiskSummaryService.hpp"
#include "../services/TeamBuilderService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class AIController : public drogon::HttpController<AIController> {
public:
    AIController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AIController::skillMatch,    "/api/manager/ai/skill-match",         drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::riskSummary,   "/api/manager/ai/risk-summary/{id}",   drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::buildTeam,     "/api/manager/ai/team-builder",        drogon::Post, "JwtMiddleware");
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

private:
    std::shared_ptr<ISkillMatchService>  skillMatchService_;
    std::shared_ptr<IRiskSummaryService> riskSummaryService_;
    std::shared_ptr<TeamBuilderService>  teamBuilderService_;
};
