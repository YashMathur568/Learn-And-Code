#include "AdminProjectScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>


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
        {"managerId",        managerId},
        {"totalStoryPoints",  storyPoints}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else {
        const auto& createdProject = resp.body.contains("data") ? resp.body["data"] : resp.body;
        ConsoleUtil::printSuccess("Project created. ID: " + std::to_string(createdProject.value("projectId", 0)));
    }
    ConsoleUtil::pause();
}


static void viewAllProjects(const ApiClient& api, bool waitForKey = true) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("ALL PROJECTS");

    const auto resp = api.get("/api/admin/projects", AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& data = resp.body.value("data", nlohmann::json::array());

    std::cout << ConsoleUtil::col("ID",   5)
              << ConsoleUtil::col("Name", 20)
              << ConsoleUtil::col("Manager", 18)
              << ConsoleUtil::col("End Date", 12)
              << ConsoleUtil::col("Status",   12)
              << ConsoleUtil::col("Health",   10)
              << "SP Done/Total\n";
    ConsoleUtil::printSeparator();

    for (const auto& project : data) {
        const int done  = project.value("completedStoryPoints", 0);
        const int total = project.value("totalStoryPoints", 0);
        std::cout << ConsoleUtil::col(std::to_string(project.value("projectId", 0)), 5)
                  << ConsoleUtil::col(ConsoleUtil::trunc(project.value("name", ""), 19), 20)
                  << ConsoleUtil::col(ConsoleUtil::trunc(project.value("managerName", ""), 17), 18)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(project.value("endDate", "")), 12)
                  << ConsoleUtil::col(project.value("status", ""), 12)
                  << ConsoleUtil::col(ConsoleUtil::healthIcon(project.value("healthStatus", "")), 10)
                  << std::to_string(done) + " / " + std::to_string(total) << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "Total: " << data.size() << "\n";
    if (waitForKey) ConsoleUtil::pause();
}


static void updateProject(const ApiClient& api) {
    viewAllProjects(api, false);
    std::cout << "\n";
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Project ID to update (B to cancel): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;


    int projectId = 0;
    try {
        projectId = std::stoi(idStr);
    } catch (...) {
        ConsoleUtil::printError("Invalid Project ID. Please enter a numeric value.");
        ConsoleUtil::pause();
        return;
    }

    const auto respGet = api.get("/api/admin/projects", AppSession::get().token);
    if (!respGet.success) { ConsoleUtil::printError(respGet.errorMessage); ConsoleUtil::pause(); return; }

    nlohmann::json found;
    for (const auto& project : respGet.body.value("data", nlohmann::json::array())) {
        if (project.value("projectId", 0) == projectId) { found = project; break; }
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
    const std::string mgrId = ConsoleUtil::promptInput("Manager ID    [" + std::to_string(found.value("managerId",0)) + "]: ");

    nlohmann::json payload;
    if (!name.empty())  payload["name"]        = name;
    if (!desc.empty())  payload["description"] = desc;
    if (!end.empty()) {
        const std::string endIso = ConsoleUtil::toIsoDate(end);
        if (endIso.empty()) {
            ConsoleUtil::printError("Invalid end date. Use DD-MM-YYYY (e.g. 31-12-2027).");
            ConsoleUtil::pause(); return;
        }
        payload["endDate"] = endIso;
    }
    if (!spStr.empty()) { try { payload["totalStoryPoints"] = std::stoi(spStr); } catch (...) {} }
    if (!mgrId.empty()) { try { payload["managerId"] = std::stoi(mgrId); } catch (...) {} }
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


static void manageMilestones(const ApiClient& api) {
    viewAllProjects(api, false);
    std::cout << "\n";
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Project ID (B to cancel): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;


    int projectId = 0;
    try {
        projectId = std::stoi(idStr);
    } catch (...) {
        ConsoleUtil::printError("Invalid Project ID. Please enter a numeric value.");
        ConsoleUtil::pause();
        return;
    }

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MILESTONES");


        const auto projResp = api.get("/api/admin/projects", AppSession::get().token);
        if (!projResp.success) { ConsoleUtil::printError(projResp.errorMessage); ConsoleUtil::pause(); return; }

        int projectTotalSP = 0;
        std::string projectName = "";
        for (const auto& projectJson : projResp.body.value("data", nlohmann::json::array())) {
            if (projectJson.value("projectId", 0) == projectId) {
                projectTotalSP = projectJson.value("totalStoryPoints", 0);
                projectName = projectJson.value("name", "");
                break;
            }
        }

        std::cout << "Project: " << projectName << " (ID: " << idStr << ")\n";
        std::cout << "Project Total Story Points: " << projectTotalSP << " SP\n\n";

        const auto resp = api.get("/api/admin/projects/" + idStr + "/milestones", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& milestones = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("#",     4)
                  << ConsoleUtil::col("Title", 24)
                  << ConsoleUtil::col("Due",   12)
                  << ConsoleUtil::col("SP",     5)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int milestoneTotalSP = 0, doneSP = 0;
        for (size_t index = 0; index < milestones.size(); ++index) {
            const auto& milestone = milestones[index];
            const int storyPoints = milestone.value("storyPoints", 0);
            milestoneTotalSP += storyPoints;
            if (milestone.value("status", "") == "DONE") doneSP += storyPoints;

            std::string title = ConsoleUtil::trunc(milestone.value("title", ""), 23);
            if (milestone.value("status", "") != "DONE") {
                const std::string due = milestone.value("dueDate", "");
                if (due < ConsoleUtil::lastMonday()) title += " ⚠";
            }

            std::cout << ConsoleUtil::col(std::to_string(index + 1), 4)
                      << ConsoleUtil::col(title, 24)
                      << ConsoleUtil::col(ConsoleUtil::fmtDate(milestone.value("dueDate", "")), 12)
                      << ConsoleUtil::col(std::to_string(storyPoints), 5)
                      << milestone.value("status", "") << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Milestones Total: " << milestoneTotalSP << " SP   |   Done: " << doneSP
                  << " SP   |   Remaining: " << (milestoneTotalSP - doneSP) << " SP\n";

        const int unallocatedSP = projectTotalSP - milestoneTotalSP;
        if (unallocatedSP > 0) {
            std::cout << "⚠  Unallocated: " << unallocatedSP << " SP (need to create more milestones)\n";
        } else if (unallocatedSP < 0) {
            std::cout << "⚠  Over-allocated by " << (-unallocatedSP) << " SP (milestones exceed project total)\n";
        }

        std::cout << "\n1. Add Milestone\n2. Update Milestone Status\n3. Back\n\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "3" || selectedOption == "b" || selectedOption == "B") return;

        if (selectedOption == "1") {
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

        } else if (selectedOption == "2" && !milestones.empty()) {
            const std::string numberString = ConsoleUtil::promptInput("Milestone # : ");
            int index = 0;
            try { index = std::stoi(numberString) - 1; }
            catch (...) {
                ConsoleUtil::printError("Please enter a valid milestone number.");
                ConsoleUtil::pause();
                continue;
            }
            if (index < 0 || index >= static_cast<int>(milestones.size())) {
                ConsoleUtil::printError("Number out of range.");
                ConsoleUtil::pause();
                continue;
            }

            const int msId = milestones[index].value("milestoneId", 0);
            std::cout << "Status: (1) NOT_STARTED  (2) IN_PROGRESS  (3) DONE\n";
            const std::string stOpt = ConsoleUtil::promptInput("Choice      : ");
            const std::vector<std::string> statuses = {"NOT_STARTED","IN_PROGRESS","DONE"};
            if (stOpt < "1" || stOpt > "3") {
                ConsoleUtil::printError("Please enter 1, 2, or 3 for the status.");
                ConsoleUtil::pause();
                continue;
            }
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
        else {
            ConsoleUtil::printError("Invalid option. Please enter 1, 2, or 3.");
            ConsoleUtil::pause();
        }
    }
}


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

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") createProject(api);
        else if (selectedOption == "2") viewAllProjects(api);
        else if (selectedOption == "3") updateProject(api);
        else if (selectedOption == "4") manageMilestones(api);
        else if (selectedOption == "5" || selectedOption == "b" || selectedOption == "B") return;
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
