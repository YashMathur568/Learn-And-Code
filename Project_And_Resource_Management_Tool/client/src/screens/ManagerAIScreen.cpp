#include "ManagerAIScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

static void skillMatchScreen(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("SKILL MATCH");

    std::cout << "Describe your project requirement in plain English:\n> ";
    std::string query;
    std::getline(std::cin, query);
    if (query.empty()) return;

    std::cout << "\nSearching... (calling AI)\n";

    const auto resp = api.post(
        "/api/manager/ai/skill-match",
        {{"query", query}},
        AppSession::get().token
    );

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("SKILL MATCH RESULTS");

    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& results = resp.body.value("data", nlohmann::json::array());
    if (results.empty()) {
        ConsoleUtil::printInfo("No matching employees found.");
    } else {
        int i = 1;
        for (const auto& r : results) {
            std::string skills;
            for (const auto& s : r.value("skills", nlohmann::json::array())) {
                if (!skills.empty()) skills += ", ";
                skills += s.value("skillName", "");
            }
            std::cout << i++ << ".  " << r.value("fullName", "")
                      << "  (Score: " << r.value("relevanceScore", 0) << "/10"
                      << "  |  " << r.value("status", "") << ")\n";
            std::cout << "    Skills : " << ConsoleUtil::trunc(skills, 55) << "\n";
            std::cout << "    Reason : " << r.value("reason", "") << "\n\n";
        }
    }
    ConsoleUtil::printInfo("Note: AI-generated. Verify before allocating.");
    ConsoleUtil::pause();
}

static void riskSummaryScreen(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("AI RISK SUMMARY");

    const auto projResp = api.get("/api/manager/projects", AppSession::get().token);
    if (!projResp.success) { ConsoleUtil::printError(projResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& projects = projResp.body.value("data", nlohmann::json::array());
    if (projects.empty()) {
        ConsoleUtil::printInfo("No projects assigned.");
        ConsoleUtil::pause();
        return;
    }

    std::cout << "Select project:\n";
    int i = 1;
    for (const auto& p : projects) {
        const std::string health = p.value("healthStatus", "ON_TRACK");
        std::cout << "  " << i++ << ".  "
                  << ConsoleUtil::trunc(p.value("name",""), 30)
                  << "  " << ConsoleUtil::healthIcon(health) << " " << health << "\n";
    }

    std::cout << "\nEnter project number: ";
    std::string sel;
    std::getline(std::cin, sel);
    int idx = 0; try { idx = std::stoi(sel) - 1; } catch (...) { return; }
    if (idx < 0 || idx >= static_cast<int>(projects.size())) return;

    const int pid = projects[idx].value("projectId", 0);
    const std::string pname = projects[idx].value("name", "");

    std::cout << "\nGenerating AI summary...\n";

    const auto resp = api.post(
        "/api/manager/ai/risk-summary/" + std::to_string(pid),
        {},
        AppSession::get().token
    );

    ConsoleUtil::clearScreen();
    std::cout << "── AI Risk Summary — " << pname << " ──\n\n";

    if (!resp.success) {
        ConsoleUtil::printError(resp.errorMessage);
    } else {
        std::cout << "\"" << resp.body.value("summary", "") << "\"\n\n";
        ConsoleUtil::printInfo("Note: AI-generated from milestone and timesheet data.");
    }
    ConsoleUtil::pause();
}

void showManagerAI(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("AI ASSISTANT");

        std::cout << "1. Skill Match    — Find best employees for a requirement\n"
                  << "2. Risk Summary   — Get a health analysis for a project\n"
                  << "3. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") skillMatchScreen(api);
        else if (opt == "2") riskSummaryScreen(api);
        else if (opt == "3" || opt == "b" || opt == "B") return;
    }
}
