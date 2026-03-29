#include "CreditCardPayment.h"
#include <iostream>

void CreditCardPayment::pay(double amount) const
{
    std::cout << "Paid " << amount << " using Credit Card" << std::endl;
}
