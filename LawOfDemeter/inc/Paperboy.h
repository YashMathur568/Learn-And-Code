#ifndef PAPERBOY_H
#define PAPERBOY_H

class Customer;

class Paperboy
{
public:
    void collectPayment(Customer& customer, double amount) const;
};

#endif
