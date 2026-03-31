#include "Wallet.h"

Wallet::Wallet(double initialAmount)
    : balance(initialAmount)
{
}

bool Wallet::canDebit(double amount) const
{
    return balance >= amount;
}

void Wallet::debit(double amount)
{
    balance -= amount;
}
