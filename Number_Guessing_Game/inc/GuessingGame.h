#ifndef GUESSING_GAME_H
#define GUESSING_GAME_H

#include <string>
#include "GuessValidator.h"

class GuessingGame
{
public:
    GuessingGame();
    void startGame();

private:
    int generateRandomNumber() const;
    std::string getValidGuess() const;
    void processGuess(int guess);

private:
    int targetNumber;
    int numberOfGuesses;
    bool hasGuessedCorrectly;

    GuessValidator validator;
};

#endif
