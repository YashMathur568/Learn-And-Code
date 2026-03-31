#include "InputValidator.h"
#include <cctype>

bool InputValidator::isValidGuess(const std::string& input)
{
    if (input.empty())
        return false;

    for (char ch : input)
    {
        if (!std::isdigit(ch))
            return false;
    }

    int value = std::stoi(input);
    return value >= 1 && value <= 100;
}
