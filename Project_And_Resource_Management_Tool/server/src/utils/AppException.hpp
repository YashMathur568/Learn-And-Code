#pragma once

#include <stdexcept>
#include <string>
#include <ctime>

// localtime_r is POSIX-only; provide a shim on Windows/MinGW
#if defined(_WIN32) && !defined(localtime_r)
inline struct tm* localtime_r(const time_t* timep, struct tm* result) {
    localtime_s(result, timep);
    return result;
}
#endif

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

class ForbiddenException : public AppException {
public:
    explicit ForbiddenException(const std::string& message)
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
