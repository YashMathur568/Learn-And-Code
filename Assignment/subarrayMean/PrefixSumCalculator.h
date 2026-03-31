#ifndef PREFIX_SUM_CALCULATOR_H
#define PREFIX_SUM_CALCULATOR_H

#include <vector>

class PrefixSumCalculator
{
public:
    explicit PrefixSumCalculator(const std::vector<long long>& data);
    long long getRangeSum(int left, int right) const;

private:
    std::vector<long long> prefixSum;
};

#endif
