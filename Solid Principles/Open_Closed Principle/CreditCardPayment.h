#ifndef CREDIT_CARD_PAYMENT_H
#define CREDIT_CARD_PAYMENT_H

#include "PaymentMethod.h"

class CreditCardPayment : public PaymentMethod
{
public:
    void pay(double amount) const override;
};

#endif
