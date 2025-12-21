#include "GuessingGame.h"
#include "InputValidator.h"

#include <iostream>
#include <random>
#include <string>

GuessingGame::GuessingGame()
    : secretNumber(generateRandomNumber()), guessCount(0)
{
}

int GuessingGame::generateRandomNumber() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);
    return dist(gen);
}

void GuessingGame::start()
{
    std::string input;
    bool guessedCorrectly = false;

    std::cout << "Guess a number between 1 and 100: ";

    while (!guessedCorrectly)
    {
        std::cin >> input;

        if (!InputValidator::isValidGuess(input))
        {
            std::cout << "Invalid input. Enter a number between 1 and 100: ";
            continue;
        }

        int guess = std::stoi(input);
        guessCount++;

        if (guess < secretNumber)
        {
            std::cout << "Too low. Guess again: ";
        }
        else if (guess > secretNumber)
        {
            std::cout << "Too high. Guess again: ";
        }
        else
        {
            std::cout << "You guessed it in " << guessCount << " guesses!" << std::endl;
            guessedCorrectly = true;
        }
    }
}
