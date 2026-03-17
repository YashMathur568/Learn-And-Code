#include "CsvParser.h"
#include <fstream>
#include <sstream>

std::vector<Record> CsvParser::parse(const std::string& filePath)
{
    std::vector<Record> records;
    std::ifstream file(filePath);
    std::string line;
    bool isHeader = true;

    while (std::getline(file, line))
    {
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        std::stringstream lineStream(line);
        std::string id, name, valueAsString, date;

        std::getline(lineStream, id, ',');
        std::getline(lineStream, name, ',');
        std::getline(lineStream, valueAsString, ',');
        std::getline(lineStream, date, ',');

        if (id.empty() || name.empty() || valueAsString.empty())
            continue;

        double value = std::stod(valueAsString);

        records.emplace_back(id, name, value, date);
    }

    return records;
}
