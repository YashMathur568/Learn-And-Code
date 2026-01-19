#include "PaymentGateway.h"
#include <cstdio>

bool PaymentGateway::processPayment(Order& order)
{
    std::printf("Processing payment for customer %s\n", order.customerId.c_str());
    order.transactionId = "TXN123";
    order.status = OrderStatus::Paid;
    return true;
}

void PaymentGateway::refundPayment(const Order& order)
{
    std::printf("Refunding payment for transaction %s\n", order.transactionId.c_str());
}
