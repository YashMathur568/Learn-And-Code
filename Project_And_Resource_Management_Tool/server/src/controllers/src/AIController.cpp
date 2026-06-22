#include "AIController.hpp"
#include "SkillMatchService.hpp"
#include "RiskSummaryService.hpp"
#include "LLMProviderManager.hpp"
#include "ConfigLoader.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

AIController::AIController()
    : projectRepo_(std::make_shared<MySQLProjectRepository>())
    , milestoneRepo_(std::make_shared<MySQLMilestoneRepository>())
    , allocationRepo_(std::make_shared<MySQLAllocationRepository>())
    , timesheetRepo_(std::make_shared<MySQLTimesheetRepository>())
    , resourceRepo_(std::make_shared<MySQLResourceRepository>())
{}

void AIController::skillMatch(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "USE_AI_FEATURES");

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string query = jsonBody.at("query").get<std::string>();

        if (query.empty()) {
            callback(ResponseBuilder::error("query field is required.", drogon::k400BadRequest));
            return;
        }

        SkillMatchService service(LLMProviderManager::getInstance()->getCurrentAdapter());
        const auto results = service.findMatches(query);
        callback(ResponseBuilder::success({{"data", results}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    } catch (const std::exception& exception) {
        callback(ResponseBuilder::error(std::string("Internal error: ") + exception.what(), drogon::k500InternalServerError));
    }
}

void AIController::riskSummary(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id
) {
    try {
        PermissionGuard::require(request, "USE_AI_FEATURES");
        const auto claims = RoleGuard::extractClaims(request);

        RiskSummaryService service(LLMProviderManager::getInstance()->getCurrentAdapter(),
            projectRepo_, milestoneRepo_, allocationRepo_, timesheetRepo_, resourceRepo_);
        const std::string summary = service.generateSummary(id, claims.userId);
        callback(ResponseBuilder::success({{"summary", summary}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void AIController::buildTeam(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "USE_AI_FEATURES");

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string description = jsonBody.at("description").get<std::string>();

        if (description.empty()) {
            callback(ResponseBuilder::error("description field is required.", drogon::k400BadRequest));
            return;
        }

        TeamBuilderService teamBuilderService(LLMProviderManager::getInstance()->getCurrentAdapter());

        const auto teamResult = teamBuilderService.buildTeam(description);
        callback(ResponseBuilder::success({{"data", teamResult}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    } catch (const std::exception& exception) {
        callback(ResponseBuilder::error(std::string("Internal error: ") + exception.what(), drogon::k500InternalServerError));
    }
}

void AIController::getAiProvider(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "USE_AI_FEATURES");
        const auto& llm = ConfigLoader::getInstance().getConfig().llm;
        callback(ResponseBuilder::success({
            {"activeProvider", llm.activeProvider},
            {"available", nlohmann::json::array({
                nlohmann::json{{"provider","gemini"}, {"model", llm.geminiModel}, {"configured", !llm.geminiApiKey.empty()}},
                nlohmann::json{{"provider","groq"},   {"model", llm.groqModel},   {"configured", !llm.groqApiKey.empty()}},
                nlohmann::json{{"provider","gemma"},  {"model", llm.gemmaModel},  {"configured", !llm.gemmaHost.empty()}}
            })}
        }));
    } catch (const UnauthorizedException& e) {
        callback(ResponseBuilder::error(e.what(), drogon::k403Forbidden));
    } catch (const std::exception& e) {
        callback(ResponseBuilder::error(e.what(), drogon::k500InternalServerError));
    }
}

void AIController::switchAiProvider(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "USE_AI_FEATURES");
        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const std::string provider = jsonBody.at("provider").get<std::string>();

        if (provider != "gemini" && provider != "groq" && provider != "gemma") {
            callback(ResponseBuilder::error(
                "Invalid provider. Choose: gemini, groq, gemma", drogon::k400BadRequest));
            return;
        }

        const auto& llm = ConfigLoader::getInstance().getConfig().llm;
        std::string apiKey, model;
        if (provider == "gemini") { apiKey = llm.geminiApiKey; model = llm.geminiModel; }
        else if (provider == "groq") { apiKey = llm.groqApiKey;   model = llm.groqModel; }
        else                         { apiKey = llm.gemmaApiKey;  model = llm.gemmaModel; }

        const bool ok = LLMProviderManager::getInstance()->setProvider(
            provider, apiKey, model, llm.gemmaHost);
        if (!ok) {
            callback(ResponseBuilder::error(
                "Failed to switch provider. Check API key and model in config.json.",
                drogon::k500InternalServerError));
            return;
        }

        ConfigLoader::getInstance().setActiveProvider(provider);
        callback(ResponseBuilder::success({
            {"activeProvider", provider},
            {"message", "Switched to " + provider + " (" + model + "). Active for all AI requests now."}
        }));
    } catch (const UnauthorizedException& e) {
        callback(ResponseBuilder::error(e.what(), drogon::k403Forbidden));
    } catch (const nlohmann::json::exception& e) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + e.what(), drogon::k400BadRequest));
    } catch (const std::exception& e) {
        callback(ResponseBuilder::error(e.what(), drogon::k500InternalServerError));
    }
}
