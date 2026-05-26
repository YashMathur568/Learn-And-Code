#include "ConsoleInputHandler.h"
#include <iostream>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <ctime>
#include <sstream>
#if defined(_WIN32)
#  include <windows.h>
#else
#  include <termios.h>
#  include <unistd.h>
#endif

std::string ConsoleInputHandler::readString(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    if (std::cin.eof())
        throw std::runtime_error("Input stream closed");
    return value;
}

double ConsoleInputHandler::readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        double value;
        std::cin >> value;
        if (std::cin.eof())
            throw std::runtime_error("Input stream closed");
        if (!std::cin.fail() && value >= 0) {
            int nextChar = std::cin.peek();
            if (nextChar != '\n' && nextChar != ' ' && nextChar != '\t' && nextChar != EOF) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a non-negative number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a non-negative number.\n";
    }
}

int ConsoleInputHandler::readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        std::cin >> value;
        if (std::cin.eof())
            throw std::runtime_error("Input stream closed");
        if (!std::cin.fail()) {
            int nextChar = std::cin.peek();
            if (nextChar != '\n' && nextChar != ' ' && nextChar != '\t' && nextChar != EOF) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a whole number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a whole number.\n";
    }
}

int ConsoleInputHandler::readIntInRange(const std::string& prompt, int min, int max) {
    while (true) {
        int value = readInt(prompt);
        if (value >= min && value <= max)
            return value;
        std::cout << "Please choose a value between " << min << " and " << max << ".\n";
    }
}

static bool isValidCalendarDate(int year, int month, int day) {
    if (year < 1 || month < 1 || month > 12 || day < 1) return false;
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
        maxDay = 29;
    return day <= maxDay;
}

static bool parseDateParts(const std::string& dateString, int& year, int& month, int& day) {
    if (dateString.size() != 10 || dateString[4] != '-' || dateString[7] != '-') return false;
    for (int digitIndex : {0,1,2,3,5,6,8,9})
        if (!std::isdigit(static_cast<unsigned char>(dateString[digitIndex]))) return false;
    year  = std::stoi(dateString.substr(0, 4));
    month = std::stoi(dateString.substr(5, 2));
    day  = std::stoi(dateString.substr(8, 2));
    return true;
}

static std::tm today() {
    std::time_t currentTime = std::time(nullptr);
    std::tm timeStruct{};
#if defined(_WIN32)
    localtime_s(&timeStruct, &currentTime);
#else
    localtime_r(&currentTime, &timeStruct);
#endif
    return timeStruct;
}

std::string ConsoleInputHandler::readDate(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string dateString;
        std::getline(std::cin, dateString);
        if (std::cin.eof())
            throw std::runtime_error("Input stream closed");
        int year, month, day;
        if (parseDateParts(dateString, year, month, day) && isValidCalendarDate(year, month, day))
            return dateString;
        std::cout << "Invalid date. Use YYYY-MM-DD with a real calendar date.\n";
    }
}

std::string ConsoleInputHandler::readPastDate(const std::string& prompt) {
    while (true) {
        std::string dateString = readDate(prompt);
        int year, month, day;
        parseDateParts(dateString, year, month, day);
        std::tm now = today();
        int todayYear = now.tm_year + 1900, todayMonth = now.tm_mon + 1, todayDay = now.tm_mday;
        if (year < todayYear || (year == todayYear && month < todayMonth) || (year == todayYear && month == todayMonth && day <= todayDay))
            return dateString;
        std::cout << "Date cannot be in the future.\n";
    }
}

std::string ConsoleInputHandler::readPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
#if defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~ENABLE_ECHO_INPUT);
    std::getline(std::cin, password);
    SetConsoleMode(hStdin, mode);
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~static_cast<tcflag_t>(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    std::cout << '\n';
    if (std::cin.eof())
        throw std::runtime_error("Input stream closed");
    return password;
}
