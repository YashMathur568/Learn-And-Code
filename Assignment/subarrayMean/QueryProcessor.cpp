#include "QueryProcessor.h"

long long QueryProcessor::calculateFloorMean(long long sum, int count)
{
    return sum / count; // integer division gives floor
}
