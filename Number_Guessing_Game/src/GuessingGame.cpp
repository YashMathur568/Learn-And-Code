#include "GuessingGame.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

GuessingGame::GuessingGame()
    : targetNumber(generateRandomNumber()),
      numberOfGuesses(0),
      hasGuessedCorrectly(false)
{
}

int GuessingGame::generateRandomNumber() const
{
    return (std::rand() % 100) + 1;
}

std::string GuessingGame::getValidGuess() const
{
    std::string input;
    std::cout << "Guess a number between 1 and 100: ";
    std::cin >> input;

    while (!validator.isValidGuess(input))
    {
        std::cout
            << "I won't count this one. Please enter a number between 1 and 100: ";
        std::cin >> input;
    }

    return input;
}

void GuessingGame::processGuess(int guess)
{
    ++numberOfGuesses;

    if (guess < targetNumber)
    {
        std::cout << "Too low. Guess again." << std::endl;
    }
    else if (guess > targetNumber)
    {
        std::cout << "Too high. Guess again." << std::endl;
    }
    else
    {
        std::cout << "You guessed it in "
                  << numberOfGuesses
                  << " guesses!" << std::endl;

        hasGuessedCorrectly = true;
    }
}

void GuessingGame::startGame()
{
    while (!hasGuessedCorrectly)
    {
        int guess = std::stoi(getValidGuess());
        processGuess(guess);
    }
}
