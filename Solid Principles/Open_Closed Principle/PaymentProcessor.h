#ifndef PAYMENT_PROCESSOR_H
#define PAYMENT_PROCESSOR_H

#include "PaymentMethod.h"

class PaymentProcessor
{
public:
    void processPayment(const PaymentMethod& method, double amount) const;
};

#endif
