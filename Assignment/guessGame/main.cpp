#include "GuessingGame.h"

// Refactored Number Guessing Game
// Improvements made:
// Clear, meaningful names
// No global dependencies
// SRP respected
// Easier to test & extend
// Bug fixed (s parameter)

int main()
{
    GuessingGame game;
    game.start();
    return 0;
}
