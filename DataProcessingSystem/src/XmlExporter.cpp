#include "XmlExporter.h"
#include <fstream>

void XmlExporter::exportData(const std::vector<Record>& records,
                             const std::string& filePath)
{
    std::ofstream file(filePath);

    file << "<records>\n";

    for (const auto& record : records)
    {
        file << "  <record>\n";
        file << "    <id>" << record.getId() << "</id>\n";
        file << "    <name>" << record.getName() << "</name>\n";
        file << "    <value>" << record.getValue() << "</value>\n";
        file << "  </record>\n";
    }

    file << "</records>";
}
