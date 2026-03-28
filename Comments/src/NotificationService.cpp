#include "NotificationService.h"
#include <cstdio>

void NotificationService::sendOrderConfirmation(const Order& order)
{
    std::printf("Sending confirmation to customer %s\n", order.customerId.c_str());
}
