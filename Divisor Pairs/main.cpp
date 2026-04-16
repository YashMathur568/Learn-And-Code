#include <iostream>
#include "inc/DivisorCounter.h"
#include "inc/DivisorPairCounter.h"

int main() {
    int testCaseCount;
    std::cin >> testCaseCount;

    DivisorCounter divisorCounter;
    DivisorPairCounter divisorPairCounter(divisorCounter);

    for (int testIndex = 0; testIndex < testCaseCount; ++testIndex) {
        int upperLimit;
        std::cin >> upperLimit;
        std::cout << divisorPairCounter.countDivisorPairs(upperLimit) << std::endl;
    }

    return 0;
}
