#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "IParser.h"

class CsvParser : public IParser
{
public:
    std::vector<Record> parse(const std::string& filePath) override;
};

#endif
