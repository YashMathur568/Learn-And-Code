#ifndef UPI_PAYMENT_H
#define UPI_PAYMENT_H

#include "PaymentMethod.h"

class UpiPayment : public PaymentMethod
{
public:
    void pay(double amount) const override;
};

#endif
