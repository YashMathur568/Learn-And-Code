#include "ConsoleOutputHandler.h"
#include "CategoryUtils.h"
#include <iostream>
#include <iomanip>
#include <numeric>

void ConsoleOutputHandler::print(const std::string& message) {
    std::cout << message << "\n";
}

void ConsoleOutputHandler::printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << "\n";
}

void ConsoleOutputHandler::printTable(const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) {
    std::vector<size_t> columnWidths(headers.size(), 0);
    for (size_t columnIndex = 0; columnIndex < headers.size(); ++columnIndex)
        columnWidths[columnIndex] = headers[columnIndex].size();
    for (const auto& row : rows)
        for (size_t columnIndex = 0; columnIndex < row.size() && columnIndex < columnWidths.size(); ++columnIndex)
            if (row[columnIndex].size() > columnWidths[columnIndex])
                columnWidths[columnIndex] = row[columnIndex].size();
    auto printSeparator = [&]() {
        std::cout << "+";
        for (size_t width : columnWidths)
            std::cout << std::string(width + 2, '-') << "+";
        std::cout << "\n";
    };
    printSeparator();
    std::cout << "|";
    for (size_t columnIndex = 0; columnIndex < headers.size(); ++columnIndex)
        std::cout << " " << std::left << std::setw(columnWidths[columnIndex]) << headers[columnIndex] << " |";
    std::cout << "\n";
    printSeparator();
    for (const auto& row : rows) {
        std::cout << "|";
        for (size_t columnIndex = 0; columnIndex < headers.size(); ++columnIndex) {
            std::string cellValue = columnIndex < row.size() ? row[columnIndex] : "";
            std::cout << " " << std::left << std::setw(columnWidths[columnIndex]) << cellValue << " |";
        }
        std::cout << "\n";
    }
    printSeparator();
}

void ConsoleOutputHandler::printSummary(const FinancialSummary& summary) {
    std::cout << "\n=== Financial Summary ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Income:   " << summary.totalIncome   << "\n";
    std::cout << "Total Expenses: " << summary.totalExpenses << "\n";
    std::cout << "Balance:        " << summary.balance       << "\n";
    std::cout << "\nExpenses by Category:\n";
    for (const auto& entry : summary.categoryBreakdown)
        std::cout << "  " << std::left << std::setw(15) << categoryToString(entry.first) << entry.second << "\n";
    std::cout << "=========================\n";
}
