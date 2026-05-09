#pragma once

#include <string>

class ILogoutInteractor {
public:
    virtual ~ILogoutInteractor() = default;
    virtual void execute(const std::string& token) = 0;
};
