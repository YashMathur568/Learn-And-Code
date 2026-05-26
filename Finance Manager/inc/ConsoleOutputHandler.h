#pragma once

#include "IOutputHandler.h"

class ConsoleOutputHandler : public IOutputHandler {
public:
    void print(const std::string& message) override;
    void printError(const std::string& message) override;
    void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) override;
    void printSummary(const FinancialSummary& summary) override;
};
