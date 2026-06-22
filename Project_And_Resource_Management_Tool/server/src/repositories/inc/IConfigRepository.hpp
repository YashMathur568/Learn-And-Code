#pragma once

#include "SystemConfig.hpp"
#include <optional>
#include <string>
#include <vector>

class IConfigRepository {
public:
    virtual ~IConfigRepository() = default;

    virtual std::optional<SystemConfig> findByKey(const std::string& key)                    = 0;
    virtual std::vector<SystemConfig>   findAll()                                            = 0;
    virtual void                        upsert(const std::string& key,
                                               const std::string& value)                     = 0;
};
