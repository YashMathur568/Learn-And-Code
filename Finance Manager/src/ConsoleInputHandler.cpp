#include "ConsoleInputHandler.h"
#include <iostream>
#include <limits>

std::string ConsoleInputHandler::readString(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

double ConsoleInputHandler::readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        double value;
        std::cin >> value;
        if (!std::cin.fail()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number.\n";
    }
}

int ConsoleInputHandler::readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        std::cin >> value;
        if (!std::cin.fail()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a whole number.\n";
    }
}

std::string ConsoleInputHandler::readDate(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string dateValue;
        std::getline(std::cin, dateValue);
        if (dateValue.size() == 10 &&
            dateValue[4] == '-' && dateValue[7] == '-')
            return dateValue;
        std::cout << "Invalid format. Use YYYY-MM-DD.\n";
    }
}
