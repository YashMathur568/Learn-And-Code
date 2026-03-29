#include "UserReportFormatter.h"

std::string UserReportFormatter::formatReport(const User& user) const
{
    std::string report;

    report += "User Report\n";
    report += "-----------\n";
    report += "Name: " + user.getName() + "\n";
    report += "Age: " + std::to_string(user.getAge()) + "\n";

    return report;
}
