#include "OrderCalculator.h"

double OrderCalculator::calculateDiscount(double orderAmount) const
{
    return orderAmount * OrderCalculator::DISCOUNT_RATE;
}

double OrderCalculator::calculateTax(double orderAmount) const
{
    return orderAmount * OrderCalculator::TAX_RATE;
}

double OrderCalculator::calculateFinalAmount(double orderAmount) const
{
    double discountAmount = calculateDiscount(orderAmount);
    double taxAmount = calculateTax(orderAmount);

    return (orderAmount + taxAmount) - discountAmount;
}
