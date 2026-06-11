#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cppconn/connection.h>
#include <mysql_driver.h>

class ConnectionGuard;

class DatabasePool {
public:
    static DatabasePool& getInstance();

    void initialize(
        const std::string& host,
        int                port,
        const std::string& databaseName,
        const std::string& username,
        const std::string& password,
        int                poolSize = 5
    );

    ConnectionGuard acquire();

private:
    DatabasePool()  = default;
    ~DatabasePool();
    DatabasePool(const DatabasePool&)            = delete;
    DatabasePool& operator=(const DatabasePool&) = delete;

    void releaseConnection(sql::Connection* connection);

    std::queue<sql::Connection*> connections;
    std::mutex                   poolMutex;
    std::condition_variable      connectionAvailable;

    std::string storedHost;
    int         storedPort{3306};
    std::string storedDatabaseName;
    std::string storedUsername;
    std::string storedPassword;
    sql::Driver* driver{nullptr};
    bool        initialized{false};

    friend class ConnectionGuard;
};

class ConnectionGuard {
public:
    ConnectionGuard(sql::Connection* connection, DatabasePool& pool);
    ~ConnectionGuard();

    ConnectionGuard(const ConnectionGuard&)            = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;

    ConnectionGuard(ConnectionGuard&& other) noexcept;
    ConnectionGuard& operator=(ConnectionGuard&& other) noexcept;

    sql::Connection* get()        const;
    sql::Connection* operator->() const;

private:
    sql::Connection* rawConnection;
    DatabasePool*    ownerPool;
    bool             valid{true};
};
