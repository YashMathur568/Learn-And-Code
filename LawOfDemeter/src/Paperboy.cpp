#include "Paperboy.h"
#include "Customer.h"

#include <iostream>

void Paperboy::collectPayment(Customer& customer, double amount) const
{
    if (customer.pay(amount))
    {
        std::cout << "Payment of " << amount << " collected successfully." << std::endl;
    }
    else
    {
        std::cout << "Payment of " << amount << " could not be collected. Will come back later." << std::endl;
    }
}
