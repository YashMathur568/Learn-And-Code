#pragma once

#include "IConfigService.hpp"
#include "../repositories/IConfigRepository.hpp"

#include <memory>
#include <vector>

class ConfigService : public IConfigService {
public:
    explicit ConfigService(std::shared_ptr<IConfigRepository> configRepository);

    std::vector<SystemConfig> getAllConfig()                                        override;
    void                      updateConfig(const std::string& key,
                                           const std::string& value)               override;

private:
    std::shared_ptr<IConfigRepository> configRepository;

    static const std::vector<std::string> ALLOWED_KEYS;

    void validateKnownKey(const std::string& key) const;
};
