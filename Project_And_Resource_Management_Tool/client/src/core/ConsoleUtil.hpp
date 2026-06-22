#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <utility>

#ifdef _WIN32
#  include <conio.h>
#else
#  include <termios.h>
#  include <unistd.h>
#endif


#if defined(_WIN32) && !defined(localtime_r)
inline struct tm* localtime_r(const time_t* timep, struct tm* result) {
    localtime_s(result, timep);
    return result;
}
#endif

namespace ConsoleUtil {


constexpr int BOX_WIDTH = 52;


inline void clearScreen() {
    std::cout << "\033[3J\033[2J\033[H" << std::flush;
}


inline int utf8Width(const std::string& text) {
    int width = 0;
    for (size_t bytePos = 0; bytePos < text.size(); ) {
        unsigned char byte = static_cast<unsigned char>(text[bytePos]);
        // Skip ANSI escape sequences: ESC [ <params> <final-byte 0x40-0x7E>
        if (byte == 0x1B && bytePos + 1 < text.size() &&
            static_cast<unsigned char>(text[bytePos + 1]) == '[') {
            bytePos += 2;
            while (bytePos < text.size() &&
                   static_cast<unsigned char>(text[bytePos]) < 0x40)
                ++bytePos;
            if (bytePos < text.size()) ++bytePos; // skip final byte (e.g. 'm')
            continue;
        }
        if      (byte < 0x80) bytePos += 1;
        else if (byte < 0xE0) bytePos += 2;
        else if (byte < 0xF0) bytePos += 3;
        else                  bytePos += 4;
        ++width;
    }
    return width;
}


inline std::string hrDouble() {
    std::string line;
    for (int index = 0; index < BOX_WIDTH; ++index) line += "\xE2\x95\x90";
    return line;
}
inline std::string hrSingle() {
    std::string line;
    for (int index = 0; index < BOX_WIDTH; ++index) line += "\xE2\x94\x80";
    return line;
}


inline void printBoxTop() {
    std::cout << "╔" << hrDouble() << "╗\n";
}
inline void printBoxBottom() {
    std::cout << "╚" << hrDouble() << "╝\n";
}
inline void printBoxRow(const std::string& text) {
    int pad = BOX_WIDTH - utf8Width(text);
    if (pad < 0) pad = 0;
    std::cout << "║ " << text << std::string(static_cast<size_t>(pad - 1), ' ') << "║\n";
}
inline void printBoxDivider() {
    std::cout << "╠" << hrDouble() << "╣\n";
}


inline void printHeader(const std::string& title, const std::string& subtitle = "") {
    printBoxTop();
    printBoxRow(title);
    if (!subtitle.empty()) {
        printBoxRow(subtitle);
    }
    printBoxBottom();
    std::cout << "\n";
}


inline void printSeparator() {
    std::cout << hrSingle() << "\n";
}


inline std::string promptInput(const std::string& label) {
    std::cout << label;
    std::string value;
    std::getline(std::cin, value);
    return value;
}


inline std::string promptPassword(const std::string& label) {
    std::cout << label;
    std::cout.flush();
    std::string password;
#ifdef _WIN32
    int characterPressed;
    while ((characterPressed = _getch()) != '\r' && characterPressed != '\n') {
        if (characterPressed == '\b' && !password.empty()) {
            password.pop_back();
        } else if (characterPressed >= 32 && characterPressed < 127) {
            password += static_cast<char>(characterPressed);
        }
    }
    std::cout << "\n";
#else
    termios oldTerminalSettings{};
    tcgetattr(STDIN_FILENO, &oldTerminalSettings);
    termios newTerminalSettings = oldTerminalSettings;
    newTerminalSettings.c_lflag &= ~static_cast<unsigned int>(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerminalSettings);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
    std::cout << "\n";
#endif
    return password;
}


inline std::string col(const std::string& text, int width) {
    const int textWidth = utf8Width(text);
    if (textWidth >= width) {
        return text.substr(0, static_cast<size_t>(width - 1)) + " ";
    }
    return text + std::string(static_cast<size_t>(width - textWidth), ' ');
}


inline std::string currentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm     timeStructure{};
    localtime_r(&now, &timeStructure);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y  %H:%M", &timeStructure);
    return buffer;
}

inline std::string currentDate() {
    std::time_t now = std::time(nullptr);
    std::tm     timeStructure{};
    localtime_r(&now, &timeStructure);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", &timeStructure);
    return buffer;
}


inline std::string fmtDate(const std::string& isoDate) {
    if (isoDate.size() < 10) return isoDate;
    return isoDate.substr(8, 2) + "-" + isoDate.substr(5, 2) + "-" + isoDate.substr(0, 4);
}



inline std::string toIsoDate(const std::string& display) {
    const size_t delimiterPosition1 = display.find('-');
    if (delimiterPosition1 == std::string::npos) return "";
    const size_t delimiterPosition2 = display.find('-', delimiterPosition1 + 1);
    if (delimiterPosition2 == std::string::npos) return "";
    std::string day   = display.substr(0, delimiterPosition1);
    std::string month = display.substr(delimiterPosition1 + 1, delimiterPosition2 - delimiterPosition1 - 1);
    std::string year  = display.substr(delimiterPosition2 + 1);
    if (day.size()   == 1) day   = "0" + day;
    if (month.size() == 1) month = "0" + month;
    if (year.size() != 4 || day.size() != 2 || month.size() != 2) return "";
    try {
        const int dayNum = std::stoi(day), monthNum = std::stoi(month), yearNum = std::stoi(year);
        if (dayNum < 1 || dayNum > 31 || monthNum < 1 || monthNum > 12 || yearNum < 2000 || yearNum > 2100) return "";
    } catch (...) { return ""; }
    return year + "-" + month + "-" + day;
}




inline std::string toWeekMonday(const std::string& isoDate) {
    if (isoDate.size() < 10) return "";
    std::tm weekTimeStructure = {};
    std::istringstream stream(isoDate);
    stream >> std::get_time(&weekTimeStructure, "%Y-%m-%d");
    if (stream.fail()) return "";
    weekTimeStructure.tm_hour = 12;
    std::mktime(&weekTimeStructure);
    const int daysToMonday = (weekTimeStructure.tm_wday == 0) ? 6 : (weekTimeStructure.tm_wday - 1);
    const std::time_t mondayTime = std::mktime(&weekTimeStructure) - static_cast<std::time_t>(daysToMonday) * 86400;
    std::tm mondayTimeStructure = {};
    localtime_r(&mondayTime, &mondayTimeStructure);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &mondayTimeStructure);
    return buffer;
}


inline std::string lastMonday() {
    std::time_t now = std::time(nullptr);
    std::tm     nowTimeStructure{};
    localtime_r(&now, &nowTimeStructure);
    int dayOfWeek = nowTimeStructure.tm_wday == 0 ? 6 : nowTimeStructure.tm_wday - 1;
    std::time_t monday = now - static_cast<std::time_t>(dayOfWeek) * 86400;
    std::tm mondayTimeStructure{};
    localtime_r(&monday, &mondayTimeStructure);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &mondayTimeStructure);
    return buffer;
}


inline std::string todayIso() {
    std::time_t now = std::time(nullptr);
    std::tm     timeStructure{};
    localtime_r(&now, &timeStructure);
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeStructure);
    return buffer;
}


inline void printSuccess(const std::string& msg) {
    std::cout << "\n  " << msg << " ✓\n";
}
inline void printError(const std::string& msg) {
    std::cout << "\n  ✗  Error: " << msg << "\n";
}
inline void printWarning(const std::string& msg) {
    std::cout << "  ⚠  " << msg << "\n";
}
inline void printInfo(const std::string& msg) {
    std::cout << "  " << msg << "\n";
}


inline void pause() {
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}
 

inline bool confirm(const std::string& prompt) {
    std::cout << "\n" << prompt << " [Y/N]: ";
    std::string ans;
    std::getline(std::cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}


inline std::string trunc(const std::string& inputString, int width) {
    if (static_cast<int>(inputString.size()) <= width) return inputString;
    if (width <= 3) return inputString.substr(0, static_cast<size_t>(width));
    return inputString.substr(0, static_cast<size_t>(width - 3)) + "...";
}


inline std::string healthIcon(const std::string& health) {
    if (health == "ON_TRACK")  return "\033[32m●\033[0m";  // green
    if (health == "AT_RISK")   return "\033[31m●\033[0m";  // red
    if (health == "ATTENTION") return "\033[33m●\033[0m";  // yellow
    return "\033[90m●\033[0m";                             // grey
}


inline const std::vector<std::string> DEPARTMENTS = {
    "Engineering", "Product", "Design", "QA / Testing",
    "DevOps / Infrastructure", "Data & Analytics", "Security",
    "Architecture", "Project Management", "Management",
    "Administration", "HR & Operations", "Finance",
    "Sales & Business Development"
};

inline const std::vector<std::string> DESIGNATIONS = {
    "Intern", "Junior", "Mid-Level", "Senior",
    "Lead", "Principal", "Manager", "Senior Manager",
    "Director", "VP", "C-Level", "Administrator"
};


struct SkillOption { std::string name; std::string category; };

inline const std::vector<SkillOption> SKILL_OPTIONS = {
    // Backend
    {"C++",          "Backend"},
    {"Go",           "Backend"},
    {"Java",         "Backend"},
    {"Python",       "Backend"},
    {"Django",       "Backend"},
    {"Spring Boot",  "Backend"},
    {"Node.js",      "Backend"},
    {"MySQL",        "Backend"},
    {"PostgreSQL",   "Backend"},
    {"MongoDB",      "Backend"},
    {"Redis",        "Backend"},
    // Frontend
    {"HTML5",        "Frontend"},
    {"CSS",          "Frontend"},
    {"JavaScript",   "Frontend"},
    {"TypeScript",   "Frontend"},
    {"React",        "Frontend"},
    {"Angular",      "Frontend"},
    {"Vue.js",       "Frontend"},
    {"RxJS",         "Frontend"},
    // DevOps
    {"Docker",       "DevOps"},
    {"Kubernetes",   "DevOps"},
    {"AWS",          "DevOps"},
    {"Jenkins",      "DevOps"},
    {"CI/CD Pipelines", "DevOps"},
    // QA
    {"Selenium",     "QA"},
    {"Cypress",      "QA"},
    {"JUnit",        "QA"},
    {"TestNG",       "QA"},
    {"Playwright",   "QA"},
};



inline std::pair<std::string, std::string> selectSkill(
        const std::string& currentName = "") {
    while (true) {
        std::cout << "\nSkills:\n";
        std::cout << "  " << col("#", 4) << col("Skill Name", 20) << "Category\n";
        for (size_t index = 0; index < SKILL_OPTIONS.size(); ++index) {
            const std::string indicator =
                (SKILL_OPTIONS[index].name == currentName) ? "  \u2190" : "";
            std::cout << "  " << col(std::to_string(index + 1) + ".", 4)
                      << col(SKILL_OPTIONS[index].name, 20)
                      << SKILL_OPTIONS[index].category << indicator << "\n";
        }
        std::cout << "  0.  " << (currentName.empty() ? "(cancel)" : "(keep: " + currentName + ")") << "\n";
        const std::string sel = promptInput("Select #: ");
        if (sel.empty() || sel == "0") return {"", ""};
        try {
            const int selectedIdx = std::stoi(sel) - 1;
            if (selectedIdx >= 0 && selectedIdx < static_cast<int>(SKILL_OPTIONS.size()))
                return {SKILL_OPTIONS[selectedIdx].name, SKILL_OPTIONS[selectedIdx].category};
        } catch (...) {}
        std::cout << "  Invalid selection. Enter a number between 0 and "
                  << SKILL_OPTIONS.size() << ".\n";
    }
}


inline std::string selectProficiency(const std::string& current = "") {
    const std::vector<std::string> profs = {"Beginner", "Intermediate", "Advanced"};
    while (true) {
        std::cout << "\nProficiency:\n";
        for (size_t i = 0; i < profs.size(); ++i) {
            const std::string ind = (profs[i] == current) ? "  \u2190" : "";
            std::cout << "  " << col(std::to_string(i + 1) + ".", 4) << profs[i] << ind << "\n";
        }
        if (!current.empty())
            std::cout << "  0.  (keep: " << current << ")\n";
        const std::string sel = promptInput("Select #: ");
        if (!current.empty() && (sel.empty() || sel == "0")) return current;
        try {
            const int idx = std::stoi(sel) - 1;
            if (idx >= 0 && idx < static_cast<int>(profs.size())) return profs[idx];
        } catch (...) {}
        std::cout << "  Invalid selection. Enter 1, 2, or 3.\n";
    }
}



inline bool isValidEmailFormat(const std::string& email) {
    const auto atPos = email.find('@');
    return atPos != std::string::npos && atPos > 0 && atPos < email.size() - 1;
}


inline std::string selectFromList(const std::string& label,
                                   const std::vector<std::string>& options,
                                   const std::string& current) {
    while (true) {
        std::cout << "\n" << label << ":\n";
        for (size_t index = 0; index < options.size(); ++index) {
            const std::string indicator = (options[index] == current) ? "  \u2190" : "";
            std::cout << "  " << col(std::to_string(index + 1) + ".", 4)
                      << options[index] << indicator << "\n";
        }
        std::cout << "  0.  (keep: " << (current.empty() ? "none" : current) << ")\n";
        const std::string sel = promptInput("Select #: ");
        if (sel.empty() || sel == "0") return current;
        try {
            const int selectedIdx = std::stoi(sel) - 1;
            if (selectedIdx >= 0 && selectedIdx < static_cast<int>(options.size()))
                return options[selectedIdx];
        } catch (...) {}
        std::cout << "  Invalid selection. Enter a number between 0 and "
                  << options.size() << ".\n";
    }
}

}
