#pragma once
#include "ILLMAdapter.hpp"
#include "ISystemConfigRepository.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <map>

class LLMProviderManager {
private:
    std::shared_ptr<ILLMAdapter> currentAdapter_;
    std::shared_ptr<ISystemConfigRepository> configRepo_;
    mutable std::mutex adapterMutex_;


    static std::shared_ptr<LLMProviderManager> instance_;
    static std::mutex instanceMutex_;


    std::shared_ptr<ILLMAdapter> createAdapter(
        const std::string& provider,
        const std::string& apiKey,
        const std::string& model,
        const std::string& gemmaHost = ""
    );

public:
    LLMProviderManager(
        std::shared_ptr<ISystemConfigRepository> configRepo,
        const std::string& initialProvider,
        const std::string& initialApiKey,
        const std::string& initialModel,
        const std::string& initialGemmaHost = ""
    );


    static void setInstance(std::shared_ptr<LLMProviderManager> instance);
    static std::shared_ptr<LLMProviderManager> getInstance();


    std::shared_ptr<ILLMAdapter> getCurrentAdapter();


    bool reloadFromDatabase();


    bool setProvider(
        const std::string& provider,
        const std::string& apiKey,
        const std::string& model,
        const std::string& gemmaHost = ""
    );
};
