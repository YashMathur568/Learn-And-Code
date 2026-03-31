#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include "Wallet.h"

class Customer
{
public:
    Customer(const std::string& firstName,
             const std::string& lastName,
             double initialBalance);

    bool pay(double amount);

    std::string getFirstName() const;
    std::string getLastName() const;

private:
    std::string firstName;
    std::string lastName;
    Wallet wallet;
};

#endif
