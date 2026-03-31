#include <iostream>
#include <vector>

#include "PrefixSumCalculator.h"
#include "QueryProcessor.h"

int main()
{
    int numberOfElements, numberOfQueries;
    std::cin >> numberOfElements >> numberOfQueries;

    std::vector<long long> array(numberOfElements);
    for (int i = 0; i < numberOfElements; ++i)
    {
        std::cin >> array[i];
    }

    PrefixSumCalculator prefixSumCalculator(array);

    for (int i = 0; i < numberOfQueries; ++i)
    {
        int left, right;
        std::cin >> left >> right;

        long long rangeSum =
            prefixSumCalculator.getRangeSum(left, right);

        int elementCount = right - left + 1;

        long long result =
            QueryProcessor::calculateFloorMean(rangeSum, elementCount);

        std::cout << result << std::endl;
    }

    return 0;
}
