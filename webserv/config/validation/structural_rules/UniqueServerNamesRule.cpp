#include <webserv/config/validation/structural_rules/UniqueServerNamesRule.hpp>

#include <webserv/config/GlobalConfig.hpp>                                          // for GlobalConfig
#include <webserv/config/ServerConfig.hpp>                                          // for ServerConfig
#include <webserv/config/validation/ValidationResult.hpp>                           // for ValidationResult
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule
#include <webserv/log/Log.hpp> // for LOCATION, Log

#include <optional> // for optional
#include <set>      // for set
#include <string>   // for basic_string, operator+, to_string, allocator, char_traits, string, operator<=>
#include <vector>   // for vector

UniqueServerNamesRule::UniqueServerNamesRule()
    : AStructuralValidationRule("UniqueServerNamesRule", "Ensures all server blocks have unique server names")
{
}

ValidationResult UniqueServerNamesRule::validateGlobal(const GlobalConfig *config) const
{
    Log::trace(LOCATION);

    if (config == nullptr)
    {
        return ValidationResult::error("Global config is null");
    }

    std::set<std::string> serverNames;
    auto servers = config->getServerConfigs();

    for (const auto *server : servers)
    {
        if (server == nullptr)
        {
            continue;
        }

        auto serverNameOpt = server->get<std::vector<std::string>>("server_name");
        auto listenOpt = server->get<int>("listen");
        if (serverNameOpt.has_value() && listenOpt.has_value())
        {
            const auto &serverNameCandidates = serverNameOpt.value();
            int listenPort = listenOpt.value();

            for (const auto &name : serverNameCandidates)
            {
                std::string uniqueKey = name + ":" + std::to_string(listenPort);
                if (serverNames.contains(uniqueKey))
                {
                    return ValidationResult::error("Duplicate server name '" + name
                                                   + "' found in configuration on port "
                                                   + std::to_string(listenPort));
                }
                serverNames.insert(uniqueKey);
            }
            // if (serverNames.contains(serverName + ":" + std::to_string(listenPort)))
            // {
            //     return ValidationResult::error("Duplicate server name '" + serverName + "' found in configuration");
            // }

            // serverNames.insert(serverName + ":" + std::to_string(listenPort));
        }
    }

    return ValidationResult::success();
}