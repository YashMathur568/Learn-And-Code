#ifndef XML_EXPORTER_H
#define XML_EXPORTER_H

#include "IExporter.h"

class XmlExporter : public IExporter
{
public:
    void exportData(const std::vector<Record>& records,
                    const std::string& filePath) override;
};

#endif
