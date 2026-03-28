#include "OrderValidator.h"

bool OrderValidator::isInvalid(int orderId) const
{
    return orderId <= 0;
}
