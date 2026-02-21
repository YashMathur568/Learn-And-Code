#ifndef IEXPORTER_H
#define IEXPORTER_H

#include <vector>
#include <string>
#include "Record.h"

class IExporter
{
public:
    virtual ~IExporter() {}
    virtual void exportData(const std::vector<Record>& records,
                            const std::string& filePath) = 0;
};

#endif
