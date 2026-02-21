#ifndef IVALIDATOR_H
#define IVALIDATOR_H

#include <vector>
#include "Record.h"

class IValidator
{
public:
    virtual ~IValidator() {}
    virtual std::vector<Record> validate(const std::vector<Record>& records) = 0;
};

#endif
