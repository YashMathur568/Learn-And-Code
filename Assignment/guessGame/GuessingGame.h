#ifndef GUESSING_GAME_H
#define GUESSING_GAME_H

class GuessingGame
{
public:
    GuessingGame();
    void start();

private:
    int secretNumber;
    int guessCount;

    int generateRandomNumber() const;
};

#endif
