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
      statistics(statisticsReference),
      errorCount(0)
{
}

void DataProcessor::process(const std::string& inputFile,
                            const std::string& outputFile)
{
    logger.log("Parsing started");
    auto records = parser.parse(inputFile);

    logger.log("Validation started");
    size_t beforeValidation = records.size();
    records = validator.validate(records);
    errorCount = static_cast<int>(beforeValidation - records.size());

    logger.log("Transformation started");
    transformer.transform(records);

    logger.log("Statistics calculation started");
    statistics.setErrorCount(errorCount);
    statistics.calculate(records);

    logger.log("Exporting started");
    exporter.exportData(records, outputFile);

    logger.log("Processing completed");
}

int DataProcessor::getErrorCount() const
{
    return errorCount;
}
