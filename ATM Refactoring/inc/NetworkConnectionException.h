#ifndef NETWORK_CONNECTION_EXCEPTION_H
#define NETWORK_CONNECTION_EXCEPTION_H

#include <stdexcept>
#include <string>

class NetworkConnectionException : public std::runtime_error
{
public:
    explicit NetworkConnectionException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
