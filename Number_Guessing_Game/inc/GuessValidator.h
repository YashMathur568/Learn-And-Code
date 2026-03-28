#ifndef GUESS_VALIDATOR_H
#define GUESS_VALIDATOR_H

#include <string>

class GuessValidator
{
private:
    static constexpr int MIN_RANDOM_NUMBER = 1;
    static constexpr int MAX_RANDOM_NUMBER = 100;

public:
    bool isValidGuess(const std::string& input) const;
};

#endif
