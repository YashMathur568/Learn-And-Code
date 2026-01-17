#include "OrderCalculator.h"

double OrderCalculator::calculateDiscount(double orderAmount) const
{
    return orderAmount * 0.10;
}

double OrderCalculator::calculateTax(double orderAmount) const
{
    return orderAmount * 0.18;
}

double OrderCalculator::calculateFinalAmount(double orderAmount) const
{
    double discountAmount = calculateDiscount(orderAmount);
    double taxAmount = calculateTax(orderAmount);

    return (orderAmount + taxAmount) - discountAmount;
}
