#ifndef USER_REPORT_FORMATTER_H
#define USER_REPORT_FORMATTER_H

#include <string>
#include "User.h"

class UserReportFormatter
{
public:
    std::string formatReport(const User& user) const;
};

#endif
