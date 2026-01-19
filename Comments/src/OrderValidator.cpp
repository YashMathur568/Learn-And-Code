#include "OrderValidator.h"

bool OrderValidator::isValid(const Order& order)
{
    return !order.items.empty() && order.totalAmount > 0;
}
