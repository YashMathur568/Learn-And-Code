#pragma once

#include <string>

class ILLMAdapter {
public:
    virtual ~ILLMAdapter() = default;

    virtual std::string generate(const std::string& prompt) = 0;
};
