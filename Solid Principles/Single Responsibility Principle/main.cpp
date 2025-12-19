#include "User.h"
#include "UserReportFormatter.h"
#include "UserReportPrinter.h"

//Note:- Below program follows Single Responsibility Principle (SRP).
// Each class has exactly one responsibility.
// Changing report format does not affect printing logic.
// Changing output method does not affect user data.

int main()
{
    User user("Yash", 23);

    UserReportFormatter formatter;
    UserReportPrinter printer;

    std::string report = formatter.formatReport(user);
    printer.print(report);

    return 0;
}
