#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/config/validation/structural_rules/SingleDefaultServerPerPortRule.hpp>
#include <webserv/log/Log.hpp>

#include <map>
#include <optional>

SingleDefaultServerPerPortRule::SingleDefaultServerPerPortRule()
    : AStructuralValidationRule("SingleDefaultServerPerPortRule",
                                "Ensures only one default server is defined per listen port")
{
}

ValidationResult SingleDefaultServerPerPortRule::validateGlobal(const GlobalConfig *config) const
{
    Log::trace(LOCATION);
    if (config == nullptr)
    {
        return ValidationResult::error("Global config is null");
    }

    std::map<int, int> defaultCountPerPort;

    for (const auto *server : config->getServerConfigs())
    {
        if (server == nullptr) continue;
        auto listenPort = server->get<int>("listen").value_or(80);
        auto def = server->get<bool>("default");
        if (def.has_value() && def.value())
        {
            int &count = defaultCountPerPort[listenPort];
            count++;
            if (count > 1)
            {
                return ValidationResult::error("Multiple default servers already defined for port "
                                               + std::to_string(listenPort));
            }
        }
    }

    return ValidationResult::success();
}
