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
    return value >= GuessValidator::MIN_RANDOM_NUMBER && value <= GuessValidator::MAX_RANDOM_NUMBER;
}
