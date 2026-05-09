#pragma once

#include <string>
#include "Session.h"

class ISessionRepository {
public:
    virtual ~ISessionRepository() = default;
    virtual void save(const Session& session) = 0;
    virtual Session findByToken(const std::string& token) = 0;
    virtual void deleteByToken(const std::string& token) = 0;
    virtual void deleteByUserId(int userId) = 0;
};
