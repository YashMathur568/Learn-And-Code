#include "PrefixSumCalculator.h"

PrefixSumCalculator::PrefixSumCalculator(const std::vector<long long>& data)
{
    prefixSum.resize(data.size() + 1);
    prefixSum[0] = 0;

    for (size_t i = 0; i < data.size(); ++i)
    {
        prefixSum[i + 1] = prefixSum[i] + data[i];
    }
}

long long PrefixSumCalculator::getRangeSum(int left, int right) const
{
    // left and right are 1-based indices
    return prefixSum[right] - prefixSum[left - 1];
}
