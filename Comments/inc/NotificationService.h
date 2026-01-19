#pragma once
#include "Order.h"

class NotificationService
{
public:
    void sendOrderConfirmation(const Order& order);
};
