#pragma once

#include "../services/SkillMatchService.hpp"
#include "../services/RiskSummaryService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class AIController : public drogon::HttpController<AIController> {
public:
    AIController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AIController::skillMatch,    "/api/manager/ai/skill-match",         drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(AIController::riskSummary,   "/api/manager/ai/risk-summary/{id}",   drogon::Post, "JwtMiddleware");
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

private:
    std::shared_ptr<SkillMatchService>  skillMatchService_;
    std::shared_ptr<RiskSummaryService> riskSummaryService_;
};
