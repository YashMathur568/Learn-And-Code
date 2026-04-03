#ifndef INSUFFICIENT_FUNDS_EXCEPTION_H
#define INSUFFICIENT_FUNDS_EXCEPTION_H

#include <stdexcept>
#include <string>

class InsufficientFundsException : public std::runtime_error
{
public:
    explicit InsufficientFundsException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
