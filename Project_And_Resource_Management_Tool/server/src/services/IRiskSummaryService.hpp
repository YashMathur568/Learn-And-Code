#pragma once

#include <string>

class IRiskSummaryService {
public:
    virtual ~IRiskSummaryService() = default;

    virtual std::string generateSummary(int projectId, int managerEmployeeId) = 0;
};
