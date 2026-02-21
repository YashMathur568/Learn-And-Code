#include "CsvExporter.h"
#include <fstream>

void CsvExporter::exportData(const std::vector<Record>& records,
                             const std::string& filePath)
{
    std::ofstream outputFile(filePath);

    outputFile << "ID,NAME,VALUE,DATE,DOUBLED_VALUE,SQUARED_VALUE\n";

    for (const auto& record : records)
    {
        outputFile << record.getId() << ","
             << record.getName() << ","
             << record.getValue() << ","
             << record.getDate() << ","
             << record.getDoubledValue() << ","
             << record.getSquaredValue() << "\n";
    }
}
