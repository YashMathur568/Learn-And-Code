#include "CsvParser.h"
#include "RecordValidator.h"
#include "RecordTransformer.h"
#include "CsvExporter.h"
#include "JsonExporter.h"
#include "XmlExporter.h"
#include "FileLogger.h"
#include "StatisticsCalculator.h"
#include "DataProcessor.h"

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

    return 0;
}
