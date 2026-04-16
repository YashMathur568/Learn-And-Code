#ifndef DIVISOR_PAIR_COUNTER_H
#define DIVISOR_PAIR_COUNTER_H

#include "../interfaces/IDivisorPairCounter.h"
#include "../interfaces/IDivisorCounter.h"

class DivisorPairCounter : public IDivisorPairCounter {
public:
    explicit DivisorPairCounter(const IDivisorCounter& divisorCounter);
    int countDivisorPairs(int upperLimit) const override;

private:
    const IDivisorCounter& divisorCounter;
};

#endif
