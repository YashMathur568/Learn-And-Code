#pragma once
#include "Order.h"

class PaymentGateway
{
public:
    bool processPayment(Order& order);
    void refundPayment(const Order& order);
};
