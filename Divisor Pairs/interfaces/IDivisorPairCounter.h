#ifndef IDIVISOR_PAIR_COUNTER_H
#define IDIVISOR_PAIR_COUNTER_H

class IDivisorPairCounter {
public:
    virtual ~IDivisorPairCounter() = default;
    virtual int countDivisorPairs(int upperLimit) const = 0;
};

#endif
