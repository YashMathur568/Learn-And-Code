#ifndef IDIVISOR_COUNTER_H
#define IDIVISOR_COUNTER_H

class IDivisorCounter {
public:
    virtual ~IDivisorCounter() = default;
    virtual int countDivisors(int number) const = 0;
};

#endif
