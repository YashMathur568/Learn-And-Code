#include "CsvParser.h"
#include "RecordValidator.h"
#include "RecordTransformer.h"
#include "CsvExporter.h"
#include "JsonExporter.h"
#include "XmlExporter.h"
#include "FileLogger.h"
#include "StatisticsCalculator.h"
#include "DataProcessor.h"
#include <iostream>

int main()
{
    CsvParser parser;
    RecordValidator validator;
    RecordTransformer transformer;
    CsvExporter exporter;
    FileLogger logger("log.txt");
    StatisticsCalculator stats;

    DataProcessor processor(parser,
                            validator,
                            transformer,
                            exporter,
                            logger,
                            stats);

    processor.process("input.csv", "output.csv");

    // show error statistics
    std::cout << "Errors encountered: " << processor.getErrorCount() << "\n";

    // statistics object was passed by reference, we can query it directly
    std::cout << "Total records processed: " << stats.getTotalRecords() << "\n";
    std::cout << "Error count (from stats): " << stats.getErrorCount() << "\n";
    std::cout << "Total value: " << stats.getTotalValue() << "\n";
    std::cout << "Average value: " << stats.getAverageValue() << "\n";

    return 0;
}
