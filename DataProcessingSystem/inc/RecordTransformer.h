#ifndef RECORD_TRANSFORMER_H
#define RECORD_TRANSFORMER_H

#include "ITransformer.h"

class RecordTransformer : public ITransformer
{
public:
    void transform(std::vector<Record>& records) override;
};

#endif
