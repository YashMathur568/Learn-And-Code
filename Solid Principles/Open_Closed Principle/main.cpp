#include "PaymentProcessor.h"
#include "CreditCardPayment.h"
#include "UpiPayment.h"

//This program demonstrates the Open–Closed Principle by allowing new payment methods to be added through inheritance without modifying the existing payment processing logic

int main()
{
    PaymentProcessor processor;

    CreditCardPayment creditCard;
    UpiPayment upi;

    processor.processPayment(creditCard, 1000.0);
    processor.processPayment(upi, 500.0);

    return 0;
}
