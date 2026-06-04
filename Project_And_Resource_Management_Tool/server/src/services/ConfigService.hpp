#pragma once

#include "../models/SystemConfig.hpp"
#include "../repositories/IConfigRepository.hpp"

#include <memory>
#include <vector>

class ConfigService {
public:
    explicit ConfigService(std::shared_ptr<IConfigRepository> configRepository);

    std::vector<SystemConfig> getAllConfig();
    void                      updateConfig(const std::string& key, const std::string& value);

private:
    std::shared_ptr<IConfigRepository> configRepository;

    static const std::vector<std::string> ALLOWED_KEYS;

    void validateKnownKey(const std::string& key) const;
};
