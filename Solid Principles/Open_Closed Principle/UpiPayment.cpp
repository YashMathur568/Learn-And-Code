#include "UpiPayment.h"
#include <iostream>

void UpiPayment::pay(double amount) const
{
    std::cout << "Paid " << amount << " using UPI" << std::endl;
}
