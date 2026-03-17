#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "IExporter.h"

class CsvExporter : public IExporter
{
public:
    void exportData(const std::vector<Record>& records,
                    const std::string& filePath) override;
};

#endif
