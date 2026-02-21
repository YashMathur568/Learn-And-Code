#ifndef ITRANSFORMER_H
#define ITRANSFORMER_H

#include <vector>
#include "Record.h"

class ITransformer
{
public:
    virtual ~ITransformer() {}
    virtual void transform(std::vector<Record>& records) = 0;
};

#endif
