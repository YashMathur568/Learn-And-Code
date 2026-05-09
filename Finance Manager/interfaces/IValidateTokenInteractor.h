#pragma once

#include <string>

class IValidateTokenInteractor {
public:
    virtual ~IValidateTokenInteractor() = default;
    
    virtual int execute(const std::string& token) = 0;
};
