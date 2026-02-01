#ifndef WALLET_H
#define WALLET_H

class Wallet
{
public:
    explicit Wallet(double initialAmount);

    bool canDebit(double amount) const;
    void debit(double amount);

private:
    double balance;
};

#endif
