#ifndef JSON_EXPORTER_H
#define JSON_EXPORTER_H

#include "IExporter.h"

class JsonExporter : public IExporter
{
public:
    void exportData(const std::vector<Record>& records,
                    const std::string& filePath) override;
};

#endif
