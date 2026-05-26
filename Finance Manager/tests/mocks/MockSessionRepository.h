#pragma once

#include "ISessionRepository.h"
#include <stdexcept>
#include <unordered_map>

class MockSessionRepository : public ISessionRepository {
public:
    std::unordered_map<std::string, Session> sessions;
    bool saveCalled = false;
    bool deleteByTokenCalled = false;
    bool deleteByUserIdCalled = false;

    void save(const Session& session) override {
        saveCalled = true;
        sessions[session.token] = session;
    }

    Session findByToken(const std::string& token) override {
        auto it = sessions.find(token);
        if (it == sessions.end())
            throw std::runtime_error("Session not found");
        return it->second;
    }

    void deleteByToken(const std::string& token) override {
        deleteByTokenCalled = true;
        sessions.erase(token);
    }

    void deleteByUserId(int userId) override {
        deleteByUserIdCalled = true;
        for (auto it = sessions.begin(); it != sessions.end(); ) {
            if (it->second.userId == userId)
                it = sessions.erase(it);
            else
                ++it;
        }
    }
};
