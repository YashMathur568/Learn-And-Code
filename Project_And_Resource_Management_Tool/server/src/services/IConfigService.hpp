#pragma once

#include "../models/SystemConfig.hpp"

#include <string>
#include <vector>

class IConfigService {
public:
    virtual ~IConfigService() = default;

    virtual std::vector<SystemConfig> getAllConfig()                                        = 0;
    virtual void                      updateConfig(const std::string& key,
                                                   const std::string& value)               = 0;
};
