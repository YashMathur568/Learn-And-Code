#include "ManagerAllocateScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>


static void showMatchResults(const nlohmann::json& results) {
    if (results.empty()) {
        ConsoleUtil::printInfo("No matching resources found.");
        ConsoleUtil::pause(); return;
    }
    ConsoleUtil::printSeparator();
    std::cout << "AI-MATCHED RESULTS\n";
    ConsoleUtil::printSeparator();
    int itemIndex = 1;
    for (const auto& matchResult : results) {
        std::string skills;
        for (const auto& skill : matchResult.value("skills", nlohmann::json::array())) {
            if (!skills.empty()) skills += ", ";
            skills += skill.value("skillName", "");
        }
        std::cout << itemIndex++ << ".  " << matchResult.value("fullName", "") << "\n";
        std::cout << "    Status : " << matchResult.value("status", "")
                  << "   Score: " << matchResult.value("relevanceScore", 0) << "/10\n";
        std::cout << "    Skills : " << ConsoleUtil::trunc(skills, 50) << "\n";
        std::cout << "    Reason : " << ConsoleUtil::trunc(matchResult.value("reason", ""), 60) << "\n\n";
    }
    ConsoleUtil::printInfo("Note: AI-generated. Use Resource ID above for Direct Allocation.");
    ConsoleUtil::printSeparator();
    ConsoleUtil::pause();
}


static void confirmAndAllocate(const ApiClient& api, int userId, const std::string& resName) {
    std::cout << "\n── " << resName << " ──\n";
    const std::string pctStr  = ConsoleUtil::promptInput("Utilisation %  : ");
    const std::string from    = ConsoleUtil::promptInput("From Date (DD-MM-YYYY): ");
    const std::string to      = ConsoleUtil::promptInput("To Date   (DD-MM-YYYY): ");
    const std::string projId  = ConsoleUtil::promptInput("Project ID     : ");

    int pct = 0, pid = 0;
    try { pct = std::stoi(pctStr); } catch (...) {}
    try { pid = std::stoi(projId); } catch (...) {}

    const std::string fromIso = ConsoleUtil::toIsoDate(from);
    const std::string toIso   = ConsoleUtil::toIsoDate(to);

    if (fromIso.empty()) {
        ConsoleUtil::printError("Invalid from date. Use DD-MM-YYYY (e.g. 01-07-2026).");
        ConsoleUtil::pause(); return;
    }
    if (toIso.empty()) {
        ConsoleUtil::printError("Invalid to date. Use DD-MM-YYYY (e.g. 31-12-2026).");
        ConsoleUtil::pause(); return;
    }
    if (fromIso >= toIso) {
        ConsoleUtil::printError("From date must be before to date.");
        ConsoleUtil::pause(); return;
    }

    std::cout << "\nValidating...\n";

    const auto resp = api.post("/api/manager/allocations", {
        {"userId",       userId},
        {"projectId",   pid},
        {"utilisation", pct},
        {"fromDate",    fromIso},
        {"toDate",      toIso}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else {
        const auto& allocation = resp.body.contains("data") ? resp.body["data"] : resp.body;
        ConsoleUtil::printSuccess(
            resName + " → Project " + std::to_string(allocation.value("projectId", pid))
            + " (" + std::to_string(allocation.value("allocationPercentage", pct)) + "%)"
        );
    }
    ConsoleUtil::pause();
}


static void aiAssistedAllocate(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("FIND RESOURCE USING AI");

    std::cout << "Describe your requirement:\n> ";
    std::string query;
    std::getline(std::cin, query);
    if (query.empty()) return;

    std::cout << "\nSearching... (AI matching in progress)\n";

    const auto resp = api.post("/api/manager/ai/skill-match",
        {{"query", query}},
        AppSession::get().token
    );

    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("AI-MATCHED RESULTS");

    const auto& results = resp.body.value("data", nlohmann::json::array());
    showMatchResults(results);
}


static void directAllocate(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DIRECT ALLOCATION");

    const std::string idStr = ConsoleUtil::promptInput("Resource ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    const auto empResp = api.get("/api/manager/resources/" + idStr, AppSession::get().token);
    if (!empResp.success) { ConsoleUtil::printError(empResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& data         = empResp.body.contains("data") ? empResp.body["data"] : empResp.body;
    const auto& resourceData  = data.contains("resource") ? data["resource"] : data;
    const std::string resName = resourceData.value("fullName", "");
    const int         resId   = resourceData.value("userId", 0);

    confirmAndAllocate(api, resId, resName);
}


static void endAllocation(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("END ALLOCATION");

    const std::string projId = ConsoleUtil::promptInput("Project ID: ");
    if (projId.empty() || projId == "b" || projId == "B") return;

    const auto projResp = api.get("/api/manager/projects/" + projId, AppSession::get().token);
    if (!projResp.success) { ConsoleUtil::printError(projResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& proj    = projResp.body.contains("data") ? projResp.body["data"] : projResp.body;
    const auto& allocs  = proj.value("allocations", nlohmann::json::array());

    std::cout << "\nActive Allocations on " << proj.value("name", "") << ":\n";
    ConsoleUtil::printSeparator();
    std::cout << ConsoleUtil::col("#",    4)
              << ConsoleUtil::col("Resource", 22)
              << ConsoleUtil::col("%",    5)
              << ConsoleUtil::col("From", 12)
              << "To\n";
    ConsoleUtil::printSeparator();

    std::vector<int> allocIds;
    int itemIndex = 1;
    for (const auto& allocation : allocs) {
        if (!allocation.value("isActive", false)) continue;
        allocIds.push_back(allocation.value("allocationId", 0));
        std::cout << ConsoleUtil::col(std::to_string(itemIndex++), 4)
                  << ConsoleUtil::col(allocation.value("resourceName", ""), 22)
                  << ConsoleUtil::col(std::to_string(allocation.value("allocationPercentage", 0)) + "%", 5)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("fromDate", "")), 12)
                  << ConsoleUtil::fmtDate(allocation.value("toDate", "")) << "\n";
    }

    if (allocIds.empty()) { ConsoleUtil::printInfo("No active allocations."); ConsoleUtil::pause(); return; }

    const std::string selStr = ConsoleUtil::promptInput("\nSelect # to end: ");
    int index = 0; try { index = std::stoi(selStr) - 1; } catch (...) { return; }
    if (index < 0 || index >= static_cast<int>(allocIds.size())) return;

    const int allocId = allocIds[index];
    if (!ConsoleUtil::confirm("End allocation " + std::to_string(allocId) + " today?")) return;

    const auto resp = api.put(
        "/api/manager/allocations/" + std::to_string(allocId) + "/end",
        {},
        AppSession::get().token
    );
    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Allocation ended.");
    ConsoleUtil::pause();
}


void showManagerAllocate(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALLOCATE RESOURCE");

        std::cout << "1. Find resource using AI (recommended)\n"
                  << "2. Allocate directly (I already know who I want)\n"
                  << "3. End an existing allocation\n"
                  << "4. Back\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") aiAssistedAllocate(api);
        else if (selectedOption == "2") directAllocate(api);
        else if (selectedOption == "3") endAllocation(api);
        else if (selectedOption == "4" || selectedOption == "b" || selectedOption == "B") return;
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
