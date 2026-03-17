#include "JsonExporter.h"
#include <fstream>

void JsonExporter::exportData(const std::vector<Record>& records,
                              const std::string& filePath)
{
    std::ofstream file(filePath);
    file << "[\n";

    for (size_t index = 0; index < records.size(); ++index)
    {
        const auto& record = records[index];
        file << "  {\n";
        file << "    \"id\": \"" << record.getId() << "\",\n";
        file << "    \"name\": \"" << record.getName() << "\",\n";
        file << "    \"value\": " << record.getValue() << "\n";
        file << "  }";

        if (index < records.size() - 1)
            file << ",";

        file << "\n";
    }

    file << "]";
}
