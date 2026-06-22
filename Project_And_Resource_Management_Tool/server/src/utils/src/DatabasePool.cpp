#include "DatabasePool.hpp"
#include "AppException.hpp"

#include <mysql_driver.h>
#include <cppconn/exception.h>

DatabasePool::~DatabasePool() {
    std::lock_guard<std::mutex> lock(poolMutex);
    while (!connections.empty()) {
        delete connections.front();
        connections.pop();
    }
}

DatabasePool& DatabasePool::getInstance() {
    static DatabasePool instance;
    return instance;
}

void DatabasePool::initialize(
    const std::string& host,
    int                port,
    const std::string& databaseName,
    const std::string& username,
    const std::string& password,
    int                poolSize
) {
    std::lock_guard<std::mutex> lock(poolMutex);

    if (initialized) {
        throw AppException("DatabasePool is already initialized.");
    }

    storedHost         = host;
    storedPort         = port;
    storedDatabaseName = databaseName;
    storedUsername     = username;
    storedPassword     = password;

    const std::string connectionUrl = "tcp://" + host + ":" + std::to_string(port);

    driver = sql::mysql::get_mysql_driver_instance();

    try {
        for (int index = 0; index < poolSize; ++index) {
            sql::Connection* connection = driver->connect(connectionUrl, username, password);
            connection->setSchema(databaseName);
            connections.push(connection);
        }
    } catch (const sql::SQLException& sqlException) {
        throw AppException(
            std::string("Failed to establish database connection: ") + sqlException.what()
        );
    }

    initialized = true;
}

ConnectionGuard DatabasePool::acquire() {
    std::unique_lock<std::mutex> lock(poolMutex);
    connectionAvailable.wait(lock, [this] { return !connections.empty(); });

    sql::Connection* connection = connections.front();
    connections.pop();

    if (connection->isClosed()) {
        try {
            const std::string connectionUrl =
                "tcp://" + storedHost + ":" + std::to_string(storedPort);

            delete connection;
            connection = driver->connect(connectionUrl, storedUsername, storedPassword);
            connection->setSchema(storedDatabaseName);
        } catch (const sql::SQLException& sqlException) {
            throw AppException(
                std::string("Failed to reconnect to database: ") + sqlException.what()
            );
        }
    }

    return ConnectionGuard(connection, *this);
}

void DatabasePool::releaseConnection(sql::Connection* connection) {
    std::lock_guard<std::mutex> lock(poolMutex);
    connections.push(connection);
    connectionAvailable.notify_one();
}

ConnectionGuard::ConnectionGuard(sql::Connection* connection, DatabasePool& pool)
    : rawConnection(connection), ownerPool(&pool), valid(true) {}

ConnectionGuard::~ConnectionGuard() {
    if (valid && rawConnection != nullptr) {
        ownerPool->releaseConnection(rawConnection);
    }
}

ConnectionGuard::ConnectionGuard(ConnectionGuard&& other) noexcept
    : rawConnection(other.rawConnection), ownerPool(other.ownerPool), valid(other.valid) {
    other.rawConnection = nullptr;
    other.valid         = false;
}

ConnectionGuard& ConnectionGuard::operator=(ConnectionGuard&& other) noexcept {
    if (this != &other) {
        if (valid && rawConnection != nullptr) {
            ownerPool->releaseConnection(rawConnection);
        }
        rawConnection = other.rawConnection;
        ownerPool     = other.ownerPool;
        valid         = other.valid;
        other.rawConnection = nullptr;
        other.valid         = false;
    }
    return *this;
}

sql::Connection* ConnectionGuard::get() const {
    return rawConnection;
}

sql::Connection* ConnectionGuard::operator->() const {
    return rawConnection;
}
