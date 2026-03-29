#ifndef PAYMENT_METHOD_H
#define PAYMENT_METHOD_H

class PaymentMethod
{
public:
    virtual ~PaymentMethod() {}
    virtual void pay(double amount) const = 0;
};

#endif
