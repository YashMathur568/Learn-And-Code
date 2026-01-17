#ifndef ORDER_CALCULATOR_H
#define ORDER_CALCULATOR_H

class OrderCalculator
{
public:
    double calculateFinalAmount(double orderAmount) const;

private:
    double calculateDiscount(double orderAmount) const;
    double calculateTax(double orderAmount) const;
};

#endif
