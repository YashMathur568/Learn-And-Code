#include "PaymentProcessor.h"

void PaymentProcessor::processPayment(const PaymentMethod& method,
                                      double amount) const
{
    method.pay(amount);
}
