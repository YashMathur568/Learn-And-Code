#ifndef RECORD_VALIDATOR_H
#define RECORD_VALIDATOR_H

#include "IValidator.h"

class RecordValidator : public IValidator
{
public:
    std::vector<Record> validate(const std::vector<Record>& records) override;
};

#endif
