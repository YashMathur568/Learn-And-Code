#ifndef DIVISOR_COUNTER_H
#define DIVISOR_COUNTER_H

#include "../interfaces/IDivisorCounter.h"

class DivisorCounter : public IDivisorCounter {
public:
    int countDivisors(int number) const override;
};

#endif
