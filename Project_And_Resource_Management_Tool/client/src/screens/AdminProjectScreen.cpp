#include "AdminProjectScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

// ── Create project ────────────────────────────────────────────────────────────
static void createProject(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("CREATE PROJECT");

    const std::string name  = ConsoleUtil::promptInput("Project Name       : ");
    if (name.empty() || name == "b" || name == "B") return;
    const std::string desc  = ConsoleUtil::promptInput("Description        : ");
    const std::string start = ConsoleUtil::promptInput("Start Date (DD-MM-YYYY): ");
    const std::string end   = ConsoleUtil::promptInput("End Date   (DD-MM-YYYY): ");

    std::cout << "Status: (1) PLANNED  (2) ACTIVE  (3) ON_HOLD\n";
    const std::string stOpt = ConsoleUtil::promptInput("Choice             : ");
    const std::vector<std::string> statuses = {"PLANNED","ACTIVE","ON_HOLD"};
    const std::string status = (stOpt >= "1" && stOpt <= "3") ? statuses[std::stoi(stOpt) - 1] : "PLANNED";

    const std::string mgrId = ConsoleUtil::promptInput("Manager ID         : ");
    const std::string spStr = ConsoleUtil::promptInput("Total Story Points : ");

    int managerId    = 0;
    int storyPoints  = 0;
    try { managerId   = std::stoi(mgrId); } catch (...) {}
    try { storyPoints = std::stoi(spStr); } catch (...) {}

    const auto resp = api.post("/api/admin/projects", {
        {"name",              name},
        {"description",       desc},
        {"startDate",         ConsoleUtil::toIsoDate(start)},
        {"endDate",           ConsoleUtil::toIsoDate(end)},
        {"status",            status},
        {"managerEmployeeId", managerId},
        {"totalStoryPoints",  storyPoints}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else {
        const auto& p = resp.body.contains("data") ? resp.body["data"] : resp.body;
        ConsoleUtil::printSuccess("Project created. ID: " + std::to_string(p.value("projectId", 0)));
    }
    ConsoleUtil::pause();
}

// ── View all projects ─────────────────────────────────────────────────────────
static void viewAllProjects(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("ALL PROJECTS");

    const auto resp = api.get("/api/admin/projects", AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& data = resp.body.value("data", nlohmann::json::array());

    std::cout << ConsoleUtil::col("ID",   5)
              << ConsoleUtil::col("Name", 22)
              << ConsoleUtil::col("End Date", 12)
              << ConsoleUtil::col("Status",   12)
              << ConsoleUtil::col("Health",   10)
              << "SP Done/Total\n";
    ConsoleUtil::printSeparator();

    for (const auto& p : data) {
        const int done  = p.value("completedStoryPoints", 0);
        const int total = p.value("totalStoryPoints", 0);
        std::cout << ConsoleUtil::col(std::to_string(p.value("projectId", 0)), 5)
                  << ConsoleUtil::col(ConsoleUtil::trunc(p.value("name", ""), 21), 22)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(p.value("endDate", "")), 12)
                  << ConsoleUtil::col(p.value("status", ""), 12)
                  << ConsoleUtil::col(ConsoleUtil::healthIcon(p.value("healthStatus", "")), 10)
                  << std::to_string(done) + " / " + std::to_string(total) << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "Total: " << data.size() << "\n";
    ConsoleUtil::pause();
}

// ── Update project ────────────────────────────────────────────────────────────
static void updateProject(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("UPDATE PROJECT DETAILS");

    const std::string idStr = ConsoleUtil::promptInput("Project ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    const auto respGet = api.get("/api/admin/projects", AppSession::get().token);
    if (!respGet.success) { ConsoleUtil::printError(respGet.errorMessage); ConsoleUtil::pause(); return; }

    nlohmann::json found;
    for (const auto& p : respGet.body.value("data", nlohmann::json::array())) {
        if (std::to_string(p.value("projectId", 0)) == idStr) { found = p; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Project not found."); ConsoleUtil::pause(); return; }

    std::cout << "\n── " << found.value("name", "") << " ──\n";
    std::cout << "Leave blank to keep current value.\n\n";

    const std::string name  = ConsoleUtil::promptInput("Name          [" + found.value("name","") + "]: ");
    const std::string desc  = ConsoleUtil::promptInput("Description   [" + ConsoleUtil::trunc(found.value("description",""),30) + "]: ");
    const std::string end   = ConsoleUtil::promptInput("End Date      [" + ConsoleUtil::fmtDate(found.value("endDate","")) + "] (DD-MM-YYYY): ");
    std::cout << "Status: (1) PLANNED  (2) ACTIVE  (3) ON_HOLD  (4) COMPLETED  (blank=keep)\n";
    const std::string stOpt = ConsoleUtil::promptInput("Choice: ");
    const std::string spStr = ConsoleUtil::promptInput("Story Points  [" + std::to_string(found.value("totalStoryPoints",0)) + "]: ");
    const std::string mgrId = ConsoleUtil::promptInput("Manager ID    [" + std::to_string(found.value("managerEmployeeId",0)) + "]: ");

    nlohmann::json payload;
    if (!name.empty())  payload["name"]              = name;
    if (!desc.empty())  payload["description"]       = desc;
    if (!end.empty())   payload["endDate"]           = ConsoleUtil::toIsoDate(end);
    if (!spStr.empty()) { try { payload["totalStoryPoints"] = std::stoi(spStr); } catch (...) {} }
    if (!mgrId.empty()) { try { payload["managerEmployeeId"] = std::stoi(mgrId); } catch (...) {} }
    if (!stOpt.empty()) {
        const std::vector<std::string> statuses = {"PLANNED","ACTIVE","ON_HOLD","COMPLETED"};
        int si = 0; try { si = std::stoi(stOpt) - 1; } catch (...) {}
        if (si >= 0 && si < 4) payload["status"] = statuses[si];
    }

    if (payload.empty()) { ConsoleUtil::printInfo("No changes."); ConsoleUtil::pause(); return; }

    const auto resp = api.put("/api/admin/projects/" + idStr, payload, AppSession::get().token);
    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Project updated.");
    ConsoleUtil::pause();
}

// ── Manage milestones ─────────────────────────────────────────────────────────
static void manageMilestones(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("MILESTONES");

    const std::string idStr = ConsoleUtil::promptInput("Project ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MILESTONES");
        std::cout << "Project ID: " << idStr << "\n\n";

        const auto resp = api.get("/api/admin/projects/" + idStr + "/milestones", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& ms = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("#",     4)
                  << ConsoleUtil::col("Title", 24)
                  << ConsoleUtil::col("Due",   12)
                  << ConsoleUtil::col("SP",     5)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int totalSP = 0, doneSP = 0;
        for (size_t i = 0; i < ms.size(); ++i) {
            const auto& m = ms[i];
            const int sp = m.value("storyPoints", 0);
            totalSP += sp;
            if (m.value("status", "") == "DONE") doneSP += sp;

            std::string title = ConsoleUtil::trunc(m.value("title", ""), 23);
            if (m.value("status", "") != "DONE") {
                const std::string due = m.value("dueDate", "");
                if (due < ConsoleUtil::lastMonday()) title += " ⚠";
            }

            std::cout << ConsoleUtil::col(std::to_string(i + 1), 4)
                      << ConsoleUtil::col(title, 24)
                      << ConsoleUtil::col(ConsoleUtil::fmtDate(m.value("dueDate", "")), 12)
                      << ConsoleUtil::col(std::to_string(sp), 5)
                      << m.value("status", "") << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << totalSP << " SP   |   Done: " << doneSP
                  << " SP   |   Remaining: " << (totalSP - doneSP) << " SP\n";

        std::cout << "\n1. Add Milestone\n2. Update Milestone Status\n3. Back\n\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "3" || opt == "b" || opt == "B") return;

        if (opt == "1") {
            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("ADD MILESTONE");
            const std::string title  = ConsoleUtil::promptInput("Title      : ");
            if (title.empty()) continue;
            const std::string due    = ConsoleUtil::promptInput("Due Date   (DD-MM-YYYY): ");
            const std::string spStr  = ConsoleUtil::promptInput("Story Points: ");
            int sp = 0; try { sp = std::stoi(spStr); } catch (...) {}

            const auto addResp = api.post(
                "/api/admin/projects/" + idStr + "/milestones",
                {{"title", title}, {"dueDate", ConsoleUtil::toIsoDate(due)}, {"storyPoints", sp}},
                AppSession::get().token
            );
            if (!addResp.success) ConsoleUtil::printError(addResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Milestone added.");
            ConsoleUtil::pause();

        } else if (opt == "2" && !ms.empty()) {
            const std::string numStr = ConsoleUtil::promptInput("Milestone # : ");
            int idx = 0; try { idx = std::stoi(numStr) - 1; } catch (...) { continue; }
            if (idx < 0 || idx >= static_cast<int>(ms.size())) continue;

            const int msId = ms[idx].value("milestoneId", 0);
            std::cout << "Status: (1) NOT_STARTED  (2) IN_PROGRESS  (3) DONE\n";
            const std::string stOpt = ConsoleUtil::promptInput("Choice      : ");
            const std::vector<std::string> statuses = {"NOT_STARTED","IN_PROGRESS","DONE"};
            if (stOpt < "1" || stOpt > "3") continue;
            const std::string newStatus = statuses[std::stoi(stOpt) - 1];

            const auto updResp = api.put(
                "/api/admin/projects/" + idStr + "/milestones/" + std::to_string(msId),
                {{"status", newStatus}},
                AppSession::get().token
            );
            if (!updResp.success) ConsoleUtil::printError(updResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Milestone updated.");
            ConsoleUtil::pause();
        }
    }
}

// ── Main project menu ─────────────────────────────────────────────────────────
void showAdminProjectMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE PROJECTS");

        std::cout << "1. Create Project\n"
                  << "2. View All Projects\n"
                  << "3. Update Project Details\n"
                  << "4. Manage Milestones\n"
                  << "5. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") createProject(api);
        else if (opt == "2") viewAllProjects(api);
        else if (opt == "3") updateProject(api);
        else if (opt == "4") manageMilestones(api);
        else if (opt == "5" || opt == "b" || opt == "B") return;
    }
}
