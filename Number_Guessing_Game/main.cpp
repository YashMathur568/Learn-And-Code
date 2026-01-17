 #include <cstdlib>
#include <ctime>
#include "GuessingGame.h"

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    GuessingGame game;
    game.startGame();

    return 0;
}
