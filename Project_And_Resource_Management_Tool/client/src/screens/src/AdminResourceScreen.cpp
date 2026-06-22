#include "AdminResourceScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>
#include <algorithm>


static void viewAllResources(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALL RESOURCES");

        const auto resp = api.get("/api/admin/resources", AppSession::get().token);
        if (!resp.success) {
            ConsoleUtil::printError(resp.errorMessage);
            ConsoleUtil::pause();
            return;
        }

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("ID",   6)
                  << ConsoleUtil::col("Role",     10)
                  << ConsoleUtil::col("Name",     22)
                  << ConsoleUtil::col("Department", 16)
                  << ConsoleUtil::col("Status", 12)
                  << "Frozen\n";
        ConsoleUtil::printSeparator();

        int activeCount = 0, inactiveCount = 0;
        for (const auto& employee : data) {
            if (employee.value("isActive", true)) ++activeCount; else ++inactiveCount;
            const bool frozen = employee.value("isFrozen", false);
            std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                      << ConsoleUtil::col(employee.value("role", ""), 10)
                      << ConsoleUtil::col(employee.value("fullName", ""), 22)
                      << ConsoleUtil::col(employee.value("department", ""), 16)
                      << ConsoleUtil::col(employee.value("status", ""), 12)
                      << (frozen ? "[FROZEN]" : "") << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << data.size()
                  << "   |   Active: " << activeCount
                  << "   |   Inactive: " << inactiveCount << "\n";

        std::cout << "\n[B] Back\n\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);
        if (selectedOption == "b" || selectedOption == "B") return;
        ConsoleUtil::printError("Press B to go back.");
        ConsoleUtil::pause();
    }
}


static void updateResource(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("UPDATE RESOURCE");


    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << ConsoleUtil::col("Department", 22)
              << "Designation\n";
    ConsoleUtil::printSeparator();
    for (const auto& employee : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("role", ""), 10)
                  << ConsoleUtil::col(employee.value("fullName", ""), 24)
                  << ConsoleUtil::col(employee.value("department", ""), 22)
                  << employee.value("designation", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Resource ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& resource : allEmps) {
        if (std::to_string(resource.value("userId", 0)) == idStr) { found = resource; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Resource not found."); ConsoleUtil::pause(); return; }

    const std::string currentName  = found.value("fullName", "");
    const std::string currentEmail = found.value("email", "");
    const std::string currentRole  = found.value("role", "");

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("UPDATE RESOURCE");
    std::cout << "\n── " << currentName << " [" << currentRole << "] ──\n\n";

    const std::string newFullName = ConsoleUtil::promptInput("Full Name  [" + currentName + "]: ");

    std::string newEmail;
    while (true) {
        newEmail = ConsoleUtil::promptInput("Email      [" + currentEmail + "]: ");
        if (newEmail.empty()) break;
        if (ConsoleUtil::isValidEmailFormat(newEmail)) break;
        ConsoleUtil::printError("Invalid email format. Must contain '@'.");
    }

    const std::string resolvedName  = newFullName.empty() ? currentName : newFullName;
    const std::string resolvedEmail = newEmail.empty() ? currentEmail : newEmail;


    const bool isSelf = (std::to_string(AppSession::get().userId) == idStr);
    static const std::vector<std::string> ROLES = {"ADMIN", "MANAGER", "RESOURCE"};
    std::string newRole = currentRole;
    if (!isSelf) {
        newRole = ConsoleUtil::selectFromList("Role", ROLES, currentRole);
    }
    const bool roleChanged = (newRole != currentRole);


    std::string resolvedDept  = found.value("department", "");
    std::string resolvedDesig = found.value("designation", "");

    if (!roleChanged) {
        resolvedDept  = ConsoleUtil::selectFromList("Department",  ConsoleUtil::DEPARTMENTS, resolvedDept);
        resolvedDesig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, resolvedDesig);
    } else if (newRole == "ADMIN") {
        std::cout << "\n  Department / Designation will be set to Administration / Administrator.\n";
        resolvedDept  = "Administration";
        resolvedDesig = "Administrator";
    } else if (newRole == "MANAGER") {
        std::cout << "\n  Department will be set to Management.\n";
        resolvedDept  = "Management";
        resolvedDesig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, resolvedDesig);
    } else {
        resolvedDept  = ConsoleUtil::selectFromList("Department",  ConsoleUtil::DEPARTMENTS, resolvedDept);
        resolvedDesig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, resolvedDesig);
    }


    const bool profileChanged = (resolvedName  != currentName
                               || resolvedEmail != currentEmail
                               || resolvedDept  != found.value("department", "")
                               || resolvedDesig != found.value("designation", ""));
    if (!profileChanged && !roleChanged) {
        ConsoleUtil::printInfo("No changes made.");
        ConsoleUtil::pause();
        return;
    }


    if (roleChanged) {
        std::string warn = "Role will change from " + currentRole + " to " + newRole + ".";
        if (currentRole == "RESOURCE")
            warn += " All active allocations will be ended.";
        else if (currentRole == "MANAGER")
            warn += " Blocked if user is still assigned to any projects.";
        else if (currentRole == "ADMIN")
            warn += " User will lose admin access.";
        ConsoleUtil::printWarning(warn);
    }

    if (!ConsoleUtil::confirm("Apply changes to " + currentName + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }


    if (profileChanged && !roleChanged) {
        const nlohmann::json payload = {
            {"fullName",    resolvedName},
            {"email",       resolvedEmail},
            {"department",  resolvedDept  != found.value("department",  "") ? resolvedDept  : ""},
            {"designation", resolvedDesig != found.value("designation", "") ? resolvedDesig : ""}
        };
        const auto resp = api.put("/api/admin/resources/" + idStr, payload, AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }
        ConsoleUtil::printSuccess("Profile updated.");
    } else if (roleChanged && (resolvedName != currentName || resolvedEmail != currentEmail)) {

        const nlohmann::json namePayload = {
            {"fullName",    resolvedName},
            {"email",       resolvedEmail},
            {"department",  ""},
            {"designation", ""}
        };
        const auto resp = api.put("/api/admin/resources/" + idStr, namePayload, AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }
    }


    if (roleChanged) {
        nlohmann::json rolePayload = {{"newRole", newRole}};
        if (newRole == "MANAGER") {
            rolePayload["designation"] = resolvedDesig;
        } else if (newRole == "RESOURCE") {
            rolePayload["department"]  = resolvedDept;
            rolePayload["designation"] = resolvedDesig;
        }
        const auto resp = api.put("/api/admin/resources/" + idStr + "/change-role", rolePayload, AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }
        ConsoleUtil::printSuccess("Role changed to " + newRole + ".");
    }

    ConsoleUtil::pause();
}


static void deactivateResource(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DEACTIVATE RESOURCE");

    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << ConsoleUtil::col("Department", 18)
              << "Active\n";
    ConsoleUtil::printSeparator();
    for (const auto& employee : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("role", ""), 10)
                  << ConsoleUtil::col(employee.value("fullName", ""), 24)
                  << ConsoleUtil::col(employee.value("department", ""), 18)
                  << (employee.value("isActive", true) ? "Yes" : "No") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Resource ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& resource : allEmps) {
        if (std::to_string(resource.value("userId", 0)) == idStr) { found = resource; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Resource not found."); ConsoleUtil::pause(); return; }
    if (!found.value("isActive", true)) { ConsoleUtil::printError("Resource is already inactive."); ConsoleUtil::pause(); return; }
    if (std::to_string(AppSession::get().userId) == idStr) {
        ConsoleUtil::printError("You cannot deactivate your own account.");
        ConsoleUtil::pause(); return;
    }

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DEACTIVATE RESOURCE");
    const std::string name = found.value("fullName", "");
    std::cout << "\n── " << name << " ──\n";
    std::cout << "Department : " << found.value("department", "") << "\n";
    std::cout << "Status     : " << found.value("status", "") << "\n";

    const auto& allocations = found.value("allocations", nlohmann::json::array());
    if (!allocations.empty()) {
        std::cout << "\nActive Allocations:\n";
        for (const auto& allocation : allocations) {
            std::cout << "  • " << allocation.value("projectName", "")
                      << "  (" << allocation.value("allocationPercentage", 0) << "% | "
                      << allocation.value("fromDate", "") << " → " << allocation.value("toDate", "") << ")\n";
        }
    }
    std::cout << "\n";
    const std::string role = found.value("role", "");
    if (role == "RESOURCE") {
        ConsoleUtil::printWarning("This will end all active allocations and block login.");
    } else {
        ConsoleUtil::printWarning("This will block login for this user.");
    }

    if (!ConsoleUtil::confirm("Deactivate " + name + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/resources/" + idStr + "/deactivate", {}, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else {
        ConsoleUtil::printSuccess("Resource deactivated.");
        const auto& ended = resp.body.value("endedAllocations", nlohmann::json::array());
        if (!ended.empty())
            std::cout << "  Ended allocations: " << ended.size() << "\n";
    }
    ConsoleUtil::pause();
}


static void reactivateResource(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("REACTIVATE RESOURCE");

    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << "Inactive resources:\n\n";
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << "Department\n";
    ConsoleUtil::printSeparator();
    int inactiveCount = 0;
    for (const auto& employee : allEmps) {
        if (employee.value("isActive", true)) continue;
        ++inactiveCount;
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("role", ""), 10)
                  << ConsoleUtil::col(employee.value("fullName", ""), 24)
                  << employee.value("department", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    if (inactiveCount == 0) {
        ConsoleUtil::printInfo("No inactive resources.");
        ConsoleUtil::pause(); return;
    }
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Resource ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& employee : allEmps) {
        if (std::to_string(employee.value("userId", 0)) == idStr) { found = employee; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Resource not found."); ConsoleUtil::pause(); return; }
    if (found.value("isActive", false)) { ConsoleUtil::printError("Resource is already active."); ConsoleUtil::pause(); return; }

    if (!ConsoleUtil::confirm("Reactivate " + found.value("fullName", "") + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/users/" + idStr + "/reactivate", {}, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else               { ConsoleUtil::printSuccess("Resource reactivated. They can now log in."); }
    ConsoleUtil::pause();
}


static void assignManager(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("ASSIGN MANAGER");

    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }
    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());

    std::cout << "Managers:\n";
    std::cout << ConsoleUtil::col("ID",   6) << "Name\n";
    ConsoleUtil::printSeparator();
    for (const auto& employee : allEmps) {
        if (employee.value("role", "") != "MANAGER") continue;
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << employee.value("fullName", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\nResources:\n";
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Name", 24)
              << "Current Manager\n";
    ConsoleUtil::printSeparator();
    for (const auto& employee : allEmps) {
        if (employee.value("role", "") == "MANAGER") continue;
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("fullName", ""), 24)
                  << employee.value("managerName", "(none)") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string resId = ConsoleUtil::promptInput("Resource ID  (B to go back): ");
    if (resId.empty() || resId == "b" || resId == "B") return;
    try { std::stoi(resId); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }
    const std::string mgrId = ConsoleUtil::promptInput("Manager ID   : ");
    if (mgrId.empty()) { ConsoleUtil::printError("Manager ID required."); ConsoleUtil::pause(); return; }
    int mgrIdInt = 0;
    try { mgrIdInt = std::stoi(mgrId); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric Manager ID.");
        ConsoleUtil::pause(); return;
    }

    const auto resp = api.put(
        "/api/admin/resources/" + resId + "/assign-manager",
        {{"managerId", mgrIdInt}},
        AppSession::get().token
    );
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else               { ConsoleUtil::printSuccess("Manager assigned."); }
    ConsoleUtil::pause();
}


static void manageSkills(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("MANAGE RESOURCE SKILLS");

    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }
    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << "Department\n";
    ConsoleUtil::printSeparator();
    for (const auto& employee : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("role", ""), 10)
                  << ConsoleUtil::col(employee.value("fullName", ""), 24)
                  << employee.value("department", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Resource ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) { ConsoleUtil::printError("Please enter a numeric ID."); ConsoleUtil::pause(); return; }

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE SKILLS");
        std::cout << "Resource ID: " << idStr << "\n\n";

        const auto resp = api.get("/api/admin/resources/" + idStr + "/skills", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& skills = resp.body.value("data", nlohmann::json::array());
        if (skills.empty()) {
            std::cout << "  (no skills on record)\n";
        } else {
            int skillIndex = 1;
            std::cout << ConsoleUtil::col("#", 4)
                      << ConsoleUtil::col("Skill", 24)
                      << ConsoleUtil::col("Category", 14)
                      << "Proficiency\n";
            ConsoleUtil::printSeparator();
            for (const auto& skill : skills) {
                std::cout << ConsoleUtil::col(std::to_string(skillIndex++), 4)
                          << ConsoleUtil::col(skill.value("skillName", ""), 24)
                          << ConsoleUtil::col(skill.value("category", ""), 14)
                          << skill.value("proficiency", "") << "\n";
            }
        }

        std::cout << "\n1. Add Skill\n2. Update Skill\n3. Remove Skill\n4. Back\n\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "4" || selectedOption == "b" || selectedOption == "B") return;

        if (selectedOption == "1") {
            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("ADD SKILL");
            const auto [skillName, category] = ConsoleUtil::selectSkill();
            if (skillName.empty()) continue;

            const std::string proficiency = ConsoleUtil::selectProficiency();

            const auto addResp = api.post(
                "/api/admin/resources/" + idStr + "/skills",
                {{"skillName", skillName}, {"category", category}, {"proficiency", proficiency}},
                AppSession::get().token
            );
            if (!addResp.success) ConsoleUtil::printError(addResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill added.");
            ConsoleUtil::pause();

        } else if (selectedOption == "2" && !skills.empty()) {
            const std::string numberString = ConsoleUtil::promptInput("Skill # to update  : ");
            int index = 0;
            try { index = std::stoi(numberString) - 1; }
            catch (...) {
                ConsoleUtil::printError("Please enter a valid skill number.");
                ConsoleUtil::pause();
                continue;
            }
            if (index < 0 || index >= static_cast<int>(skills.size())) {
                ConsoleUtil::printError("Number out of range.");
                ConsoleUtil::pause();
                continue;
            }

            const int skillId          = skills[index].value("skillId", 0);
            const std::string currentSkillName = skills[index].value("skillName", "");
            const std::string currentCategory  = skills[index].value("category", "Other");

            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("UPDATE SKILL");
            const auto [newSkillName, newCategory] = ConsoleUtil::selectSkill(currentSkillName);
            const std::string resolvedName     = newSkillName.empty() ? currentSkillName : newSkillName;
            const std::string resolvedCategory = newSkillName.empty() ? currentCategory  : newCategory;

            const std::string proficiency = ConsoleUtil::selectProficiency(skills[index].value("proficiency", ""));

            const auto updResp = api.put(
                "/api/admin/resources/" + idStr + "/skills/" + std::to_string(skillId),
                {{"skillName", resolvedName}, {"category", resolvedCategory}, {"proficiency", proficiency}},
                AppSession::get().token
            );
            if (!updResp.success) ConsoleUtil::printError(updResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill updated.");
            ConsoleUtil::pause();

        } else if (selectedOption == "3" && !skills.empty()) {
            const std::string numberString = ConsoleUtil::promptInput("Skill # to remove  : ");
            int index = 0;
            try { index = std::stoi(numberString) - 1; }
            catch (...) {
                ConsoleUtil::printError("Please enter a valid skill number.");
                ConsoleUtil::pause();
                continue;
            }
            if (index < 0 || index >= static_cast<int>(skills.size())) {
                ConsoleUtil::printError("Number out of range.");
                ConsoleUtil::pause();
                continue;
            }

            const int skillId = skills[index].value("skillId", 0);
            const std::string skillName = skills[index].value("skillName", "");
            if (!ConsoleUtil::confirm("Remove skill \"" + skillName + "\"?")) continue;

            const auto delResp = api.del(
                "/api/admin/resources/" + idStr + "/skills/" + std::to_string(skillId),
                AppSession::get().token
            );
            if (!delResp.success) ConsoleUtil::printError(delResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill removed.");
            ConsoleUtil::pause();
        }
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}



void showAdminResourceMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE RESOURCES");

        std::cout << "1. View All Resources\n"
                  << "2. Update Resource\n"
                  << "3. Deactivate Resource\n"
                  << "4. Reactivate Resource\n"
                  << "5. Manage Resource Skills\n"
                  << "6. Assign Manager\n"
                  << "7. Back\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") viewAllResources(api);
        else if (selectedOption == "2") updateResource(api);
        else if (selectedOption == "3") deactivateResource(api);
        else if (selectedOption == "4") reactivateResource(api);
        else if (selectedOption == "5") manageSkills(api);
        else if (selectedOption == "6") assignManager(api);
        else if (selectedOption == "7" || selectedOption == "b" || selectedOption == "B") return;
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
