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

static bool isValidCalendarDate(int y, int m, int d) {
    if (y < 1 || m < 1 || m > 12 || d < 1) return false;
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
        maxDay = 29;
    return d <= maxDay;
}

static bool parseDateParts(const std::string& s, int& y, int& mo, int& d) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    for (int i : {0,1,2,3,5,6,8,9})
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    y  = std::stoi(s.substr(0, 4));
    mo = std::stoi(s.substr(5, 2));
    d  = std::stoi(s.substr(8, 2));
    return true;
}

static std::tm today() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

std::string ConsoleInputHandler::readDate(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        if (std::cin.eof())
            throw std::runtime_error("Input stream closed");
        int y, mo, d;
        if (parseDateParts(s, y, mo, d) && isValidCalendarDate(y, mo, d))
            return s;
        std::cout << "Invalid date. Use YYYY-MM-DD with a real calendar date.\n";
    }
}

std::string ConsoleInputHandler::readPastDate(const std::string& prompt) {
    while (true) {
        std::string s = readDate(prompt);
        int y, mo, d;
        parseDateParts(s, y, mo, d);
        std::tm now = today();
        int todayY = now.tm_year + 1900, todayM = now.tm_mon + 1, todayD = now.tm_mday;
        if (y < todayY || (y == todayY && mo < todayM) || (y == todayY && mo == todayM && d <= todayD))
            return s;
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
