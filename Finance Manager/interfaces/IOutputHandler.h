#pragma once

#include <string>
#include <vector>
#include "FinancialSummary.h"

class IOutputHandler {
public:
    virtual ~IOutputHandler() = default;
    virtual void print(const std::string& message) = 0;
    virtual void printError(const std::string& message) = 0;
    virtual void printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) = 0;
    virtual void printSummary(const FinancialSummary& summary) = 0;
};
