#pragma once

#include <string>

class IRegisterInteractor {
public:
    virtual ~IRegisterInteractor() = default;
    virtual void execute(const std::string& username, const std::string& password) = 0;
};
