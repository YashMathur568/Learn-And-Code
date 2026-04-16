#include "../inc/DivisorCounter.h"
#include <cmath>

int DivisorCounter::countDivisors(int number) const {
    if (number <= 0) {
        return 0;
    }

    int divisorCount = 0;
    int squareRoot = static_cast<int>(std::sqrt(number));

    for (int candidate = 1; candidate <= squareRoot; ++candidate) {
        if (number % candidate == 0) {
            ++divisorCount;
            if (candidate != number / candidate) {
                ++divisorCount;
            }
        }
    }

    return divisorCount;
}
