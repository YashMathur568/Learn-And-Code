#include "RecordTransformer.h"
#include <algorithm>

void RecordTransformer::transform(std::vector<Record>& records)
{
    for (auto& record : records)
    {
        std::string uppercaseName = record.getName();
        std::transform(uppercaseName.begin(), uppercaseName.end(), uppercaseName.begin(), ::toupper);
        record.setName(uppercaseName);

        double currentValue = record.getValue();
        double doubledValue = currentValue * 2;
        double squaredValue = currentValue * currentValue;
        record.setDerivedValues(doubledValue, squaredValue);
    }
}
