#ifndef ORDER_CALCULATOR_H
#define ORDER_CALCULATOR_H

class OrderCalculator
{
private:
    static constexpr double DISCOUNT_RATE = 0.10;
    static constexpr double TAX_RATE = 0.18;

public:
    double calculateFinalAmount(double orderAmount) const;

private:
    double calculateDiscount(double orderAmount) const;
    double calculateTax(double orderAmount) const;
};

#endif
