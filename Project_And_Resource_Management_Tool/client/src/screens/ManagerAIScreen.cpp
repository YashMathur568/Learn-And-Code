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
        int itemIndex = 1;
        for (const auto& result : results) {
            std::string skills;
            for (const auto& skill : result.value("skills", nlohmann::json::array())) {
                if (!skills.empty()) skills += ", ";
                skills += skill.value("skillName", "");
            }
            std::cout << itemIndex++ << ".  " << result.value("fullName", "")
                      << "  (Score: " << result.value("relevanceScore", 0) << "/10"
                      << "  |  " << result.value("status", "") << ")\n";
            std::cout << "    Skills : " << ConsoleUtil::trunc(skills, 55) << "\n";
            std::cout << "    Reason : " << result.value("reason", "") << "\n\n";
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
    int projectIndex = 1;
    for (const auto& project : projects) {
        const std::string health = project.value("healthStatus", "ON_TRACK");
        std::cout << "  " << projectIndex++ << ".  "
                  << ConsoleUtil::trunc(project.value("name",""), 30)
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

static void teamBuilderScreen(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("TEAM BUILDER");

    std::cout << "Describe the team you need in plain English.\n"
              << "Example: Senior Java developer, DevOps engineer with Kubernetes, QA tester\n\n"
              << "> ";
    std::string description;
    std::getline(std::cin, description);
    if (description.empty()) return;

    std::cout << "\nBuilding team... (parsing roles and matching skills)\n";

    const auto resp = api.post(
        "/api/manager/ai/team-builder",
        {{"description", description}},
        AppSession::get().token
    );

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("TEAM SUGGESTION");

    if (!resp.success) {
        ConsoleUtil::printError(resp.errorMessage);
        ConsoleUtil::pause();
        return;
    }

    const auto& roles = resp.body.value("data", nlohmann::json::array());
    if (roles.empty()) {
        ConsoleUtil::printInfo("No roles could be parsed from your description.");
        ConsoleUtil::pause();
        return;
    }

    int filled = 0;
    int total  = static_cast<int>(roles.size());

    for (int i = 0; i < total; ++i) {
        const auto& role = roles[i];
        const std::string roleName = role.value("roleName", "Role " + std::to_string(i + 1));

        if (role.value("filled", false)) {
            ++filled;
            const std::string status       = role.value("status", "");
            const std::string allocUntil   = role.value("allocatedUntil", "");
            const bool onBench             = (status == "BENCH");

            // Skill list
            std::string skills;
            for (const auto& s : role.value("matchedSkills", nlohmann::json::array())) {
                if (!skills.empty()) skills += ", ";
                skills += s.get<std::string>();
            }

            std::cout << "  \u2713  [FILLED] " << roleName << "\n";
            std::cout << "  Employee   : " << role.value("fullName", "")
                      << "  (ID: " << role.value("userId", 0) << ")\n";
            std::cout << "  Dept       : " << role.value("department", "")
                      << "  |  " << role.value("designation", "") << "\n";
            std::cout << "  Status     : " << (onBench ? "BENCH (available now)" : "ALLOCATED until " + ConsoleUtil::fmtDate(allocUntil)) << "\n";
            std::cout << "  Skills     : " << ConsoleUtil::trunc(skills, 55) << "\n";
        } else {
            std::cout << "  \u2717  [GAP]    " << roleName << "\n";
            std::cout << "  Reason     : " << role.value("gapReason", "Unknown") << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "────────────────────────────────────────────────────\n";
    std::cout << filled << "/" << total << " roles filled";
    if (filled < total) std::cout << "   |   " << (total - filled) << " gap(s) requiring action";
    std::cout << "\n\n";
    ConsoleUtil::printInfo("Note: AI-generated. Verify skills and availability before allocating.");
    ConsoleUtil::pause();
}

void showManagerAI(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("AI ASSISTANT");

        std::cout << "1. Skill Match    — Find best employees for a requirement\n"
                  << "2. Risk Summary   — Get a health analysis for a project\n"
                  << "3. Team Builder   — Suggest a full team with skill matching\n"
                  << "4. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if      (opt == "1") skillMatchScreen(api);
        else if (opt == "2") riskSummaryScreen(api);
        else if (opt == "3") teamBuilderScreen(api);
        else if (opt == "4" || opt == "b" || opt == "B") return;
    }
}
