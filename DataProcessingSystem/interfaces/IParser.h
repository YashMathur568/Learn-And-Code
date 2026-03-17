#ifndef IPARSER_H
#define IPARSER_H

#include <vector>
#include <string>
#include "Record.h"

class IParser
{
public:
    virtual ~IParser() {}
    virtual std::vector<Record> parse(const std::string& filePath) = 0;
};

#endif
