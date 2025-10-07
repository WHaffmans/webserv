#include <webserv/config/validation/structural_rules/UniqueServerNamesRule.hpp>

#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/log/Log.hpp>

#include <optional>
#include <set>
#include <string>

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

        auto serverNameOpt = server->get<std::string>("server_name");
        auto listenOpt = server->get<int>("listen");
        if (serverNameOpt.has_value() && listenOpt.has_value())
        {
            const std::string &serverName = serverNameOpt.value();
            int listenPort = listenOpt.value();

            if (serverNames.contains(serverName + ":" + std::to_string(listenPort)))
            {
                return ValidationResult::error("Duplicate server name '" + serverName + "' found in configuration");
            }

            serverNames.insert(serverName + ":" + std::to_string(listenPort));
        }
    }

    return ValidationResult::success();
}