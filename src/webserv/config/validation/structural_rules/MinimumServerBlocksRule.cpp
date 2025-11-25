#include <webserv/config/validation/structural_rules/MinimumServerBlocksRule.hpp>

#include <webserv/config/GlobalConfig.hpp>                                          // for GlobalConfig
#include <webserv/config/validation/ValidationResult.hpp>                           // for ValidationResult
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule
#include <webserv/log/Log.hpp> // for LOCATION, Log

#include <string> // for allocator, operator+, char_traits, to_string, basic_string
#include <vector> // for vector

MinimumServerBlocksRule::MinimumServerBlocksRule(size_t minimumServers)
    : AStructuralValidationRule("MinimumServerBlocksRule", "Ensures global config has at least "
                                                               + std::to_string(minimumServers) + " server block(s)"),
      minimumServers_(minimumServers)
{
}

ValidationResult MinimumServerBlocksRule::validateGlobal(const GlobalConfig *config) const
{
    Log::trace(LOCATION);

    if (config == nullptr)
    {
        return ValidationResult::error("Global config is null");
    }

    size_t serverCount = config->getServerConfigs().size();

    if (serverCount < minimumServers_)
    {
        return ValidationResult::error("Global configuration must have at least " + std::to_string(minimumServers_)
                                       + " server block(s), but found " + std::to_string(serverCount));
    }

    return ValidationResult::success();
}