#include "DataProcessor.h"

DataProcessor::DataProcessor(IParser& parserReference,
                             IValidator& validatorReference,
                             ITransformer& transformerReference,
                             IExporter& exporterReference,
                             ILogger& loggerReference,
                             IStatisticsCalculator& statisticsReference)
    : parser(parserReference),
      validator(validatorReference),
      transformer(transformerReference),
      exporter(exporterReference),
      logger(loggerReference),
      statistics(statisticsReference)
{
}

void DataProcessor::process(const std::string& inputFile,
                            const std::string& outputFile)
{
    logger.log("Parsing started");
    auto records = parser.parse(inputFile);

    logger.log("Validation started");
    records = validator.validate(records);

    logger.log("Transformation started");
    transformer.transform(records);

    logger.log("Statistics calculation started");
    statistics.calculate(records);

    logger.log("Exporting started");
    exporter.exportData(records, outputFile);

    logger.log("Processing completed");
}
