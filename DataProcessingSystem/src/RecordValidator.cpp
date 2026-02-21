#include "RecordValidator.h"

std::vector<Record> RecordValidator::validate(const std::vector<Record>& records)
{
    std::vector<Record> validRecords;

    for (const auto& record : records)
    {
        if (!record.getId().empty() &&
            !record.getName().empty())
        {
            validRecords.push_back(record);
        }
    }

    return validRecords;
}
