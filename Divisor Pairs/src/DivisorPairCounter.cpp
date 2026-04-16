#include "../inc/DivisorPairCounter.h"

DivisorPairCounter::DivisorPairCounter(const IDivisorCounter& divisorCounter)
    : divisorCounter(divisorCounter) {}

int DivisorPairCounter::countDivisorPairs(int upperLimit) const {
    if (upperLimit <= 2) {
        return 0;
    }

    int matchingPairCount = 0;

    for (int current = 2; current < upperLimit; ++current) {
        int currentDivisorCount = divisorCounter.countDivisors(current);
        int nextDivisorCount = divisorCounter.countDivisors(current + 1);

        if (currentDivisorCount == nextDivisorCount) {
            ++matchingPairCount;
        }
    }

    return matchingPairCount;
}
