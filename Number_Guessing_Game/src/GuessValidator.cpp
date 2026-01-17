#include "GuessValidator.h"
#include <cctype>

bool GuessValidator::isValidGuess(const std::string& input) const
{
    if (input.empty())
    {
        return false;
    }

    for (char inputCharacter : input)
{
    if (!std::isdigit(inputCharacter))
    {
        return false;
    }
}

    int value = std::stoi(input);
    return value >= 1 && value <= 100;
}
