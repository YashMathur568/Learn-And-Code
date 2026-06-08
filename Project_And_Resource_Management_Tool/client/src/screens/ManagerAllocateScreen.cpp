#include "ManagerAllocateScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

// ── Shared: show AI match results and let user pick ───────────────────────────
static int showMatchResults(const nlohmann::json& results) {
    if (results.empty()) {
        ConsoleUtil::printInfo("No matching employees found.");
        return -1;
    }
    ConsoleUtil::printSeparator();
    std::cout << "AI-MATCHED RESULTS\n";
    ConsoleUtil::printSeparator();
    int i = 1;
    for (const auto& r : results) {
        std::string skills;
        for (const auto& s : r.value("skills", nlohmann::json::array())) {
            if (!skills.empty()) skills += ", ";
            skills += s.value("skillName", "");
        }
        std::cout << i++ << ".  " << r.value("fullName", "") << "\n";
        std::cout << "    Status : " << r.value("status", "")
                  << "   Score: " << r.value("relevanceScore", 0) << "/10\n";
        std::cout << "    Skills : " << ConsoleUtil::trunc(skills, 50) << "\n";
        std::cout << "    Reason : " << ConsoleUtil::trunc(r.value("reason", ""), 60) << "\n\n";
    }
    ConsoleUtil::printInfo("Note: AI-generated. Verify before confirming.");
    ConsoleUtil::printSeparator();

    std::cout << "\nSelect # (or 0 to cancel): ";
    std::string sel;
    std::getline(std::cin, sel);
    int idx = 0;
    try { idx = std::stoi(sel); } catch (...) { return -1; }
    if (idx <= 0 || idx > static_cast<int>(results.size())) return -1;
    return results[idx - 1].value("employeeId", -1);
}

// ── Confirm and submit allocation ─────────────────────────────────────────────
static void confirmAndAllocate(const ApiClient& api, int employeeId, const std::string& empName) {
    std::cout << "\n── " << empName << " ──\n";
    const std::string pctStr  = ConsoleUtil::promptInput("Utilisation %  : ");
    const std::string from    = ConsoleUtil::promptInput("From Date (DD-MM-YYYY): ");
    const std::string to      = ConsoleUtil::promptInput("To Date   (DD-MM-YYYY): ");
    const std::string projId  = ConsoleUtil::promptInput("Project ID     : ");

    int pct = 0, pid = 0;
    try { pct = std::stoi(pctStr); } catch (...) {}
    try { pid = std::stoi(projId); } catch (...) {}

    std::cout << "\nValidating...\n";

    const auto resp = api.post("/api/manager/allocations", {
        {"employeeId",            employeeId},
        {"projectId",             pid},
        {"allocationPercentage",  pct},
        {"fromDate",              ConsoleUtil::toIsoDate(from)},
        {"toDate",                ConsoleUtil::toIsoDate(to)}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else {
        const auto& a = resp.body.contains("data") ? resp.body["data"] : resp.body;
        ConsoleUtil::printSuccess(
            empName + " → Project " + std::to_string(a.value("projectId", pid))
            + " (" + std::to_string(a.value("allocationPercentage", pct)) + "%)"
        );
    }
    ConsoleUtil::pause();
}

// ── AI-assisted search ────────────────────────────────────────────────────────
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
    const int empId = showMatchResults(results);
    if (empId < 0) return;

    std::string empName;
    for (const auto& r : results) {
        if (r.value("employeeId", -1) == empId) { empName = r.value("fullName", ""); break; }
    }

    confirmAndAllocate(api, empId, empName);
}

// ── Direct allocation ─────────────────────────────────────────────────────────
static void directAllocate(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DIRECT ALLOCATION");

    const std::string idStr = ConsoleUtil::promptInput("Employee ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    const auto empResp = api.get("/api/manager/employees/" + idStr, AppSession::get().token);
    if (!empResp.success) { ConsoleUtil::printError(empResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& e       = empResp.body.contains("data") ? empResp.body["data"] : empResp.body;
    const std::string   empName = e.value("fullName", "");
    const int           empId   = e.value("employeeId", 0);

    confirmAndAllocate(api, empId, empName);
}

// ── End allocation ────────────────────────────────────────────────────────────
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
              << ConsoleUtil::col("Employee", 22)
              << ConsoleUtil::col("%",    5)
              << ConsoleUtil::col("From", 12)
              << "To\n";
    ConsoleUtil::printSeparator();

    std::vector<int> allocIds;
    int i = 1;
    for (const auto& a : allocs) {
        if (!a.value("isActive", false)) continue;
        allocIds.push_back(a.value("allocationId", 0));
        std::cout << ConsoleUtil::col(std::to_string(i++), 4)
                  << ConsoleUtil::col(a.value("employeeName", ""), 22)
                  << ConsoleUtil::col(std::to_string(a.value("allocationPercentage", 0)) + "%", 5)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(a.value("fromDate", "")), 12)
                  << ConsoleUtil::fmtDate(a.value("toDate", "")) << "\n";
    }

    if (allocIds.empty()) { ConsoleUtil::printInfo("No active allocations."); ConsoleUtil::pause(); return; }

    const std::string selStr = ConsoleUtil::promptInput("\nSelect # to end: ");
    int idx = 0; try { idx = std::stoi(selStr) - 1; } catch (...) { return; }
    if (idx < 0 || idx >= static_cast<int>(allocIds.size())) return;

    const int allocId = allocIds[idx];
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

// ── Main allocate menu ────────────────────────────────────────────────────────
void showManagerAllocate(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALLOCATE RESOURCE");

        std::cout << "1. Find resource using AI (recommended)\n"
                  << "2. Allocate directly (I already know who I want)\n"
                  << "3. End an existing allocation\n"
                  << "4. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") aiAssistedAllocate(api);
        else if (opt == "2") directAllocate(api);
        else if (opt == "3") endAllocation(api);
        else if (opt == "4" || opt == "b" || opt == "B") return;
    }
}
