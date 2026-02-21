#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include "IParser.h"
#include "IValidator.h"
#include "ITransformer.h"
#include "IExporter.h"
#include "ILogger.h"
#include "IStatisticsCalculator.h"

class DataProcessor
{
private:
    IParser& parser;
    IValidator& validator;
    ITransformer& transformer;
    IExporter& exporter;
    ILogger& logger;
    IStatisticsCalculator& statistics;

public:
    DataProcessor(IParser& parserReference,
                  IValidator& validatorReference,
                  ITransformer& transformerReference,
                  IExporter& exporterReference,
                  ILogger& loggerReference,
                  IStatisticsCalculator& statisticsReference);

    void process(const std::string& inputFile,
                 const std::string& outputFile);
};

#endif
