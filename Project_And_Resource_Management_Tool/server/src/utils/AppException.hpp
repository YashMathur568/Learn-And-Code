#pragma once

#include <stdexcept>
#include <string>

class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& message)
        : std::runtime_error(message) {}
};

class NotFoundException : public AppException {
public:
    explicit NotFoundException(const std::string& message)
        : AppException(message) {}
};

class ValidationException : public AppException {
public:
    explicit ValidationException(const std::string& message)
        : AppException(message) {}
};

class UnauthorizedException : public AppException {
public:
    explicit UnauthorizedException(const std::string& message)
        : AppException(message) {}
};

class ConflictException : public AppException {
public:
    explicit ConflictException(const std::string& message)
        : AppException(message) {}
};

class ServiceUnavailableException : public AppException {
public:
    explicit ServiceUnavailableException(const std::string& message)
        : AppException(message) {}
};
