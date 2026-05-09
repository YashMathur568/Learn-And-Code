#pragma once

#include <string>

class ILoginInteractor {
public:
    virtual ~ILoginInteractor() = default;
    
    virtual std::string execute(const std::string& username, const std::string& password) = 0;
};
