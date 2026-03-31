#include "Customer.h"

Customer::Customer(const std::string& firstName,
                   const std::string& lastName,
                   double initialBalance)
    : firstName(firstName),
      lastName(lastName),
      wallet(initialBalance)
{
}

bool Customer::pay(double amount)
{
    if (!wallet.canDebit(amount))
    {
        return false;
    }

    wallet.debit(amount);
    return true;
}

std::string Customer::getFirstName() const
{
    return firstName;
}

std::string Customer::getLastName() const
{
    return lastName;
}
