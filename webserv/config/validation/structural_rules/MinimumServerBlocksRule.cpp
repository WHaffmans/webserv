#include "webserv/config/validation/structural_rules/MinimumServerBlocksRule.hpp"

#include "webserv/config/GlobalConfig.hpp"
#include "webserv/log/Log.hpp"

#include <string>

MinimumServerBlocksRule::MinimumServerBlocksRule(size_t minimumServers)
    : AStructuralValidationRule("MinimumServerBlocksRule",
                                "Ensures global config has at least " + std::to_string(minimumServers) + " server block(s)"),
      minimumServers_(minimumServers)
{
}

ValidationResult MinimumServerBlocksRule::validateGlobal(const GlobalConfig *config) const
{
    Log::trace(LOCATION);

    if (config == nullptr) {
        return ValidationResult::error("Global config is null");
    }

    size_t serverCount = config->getServerConfigs().size();

    if (serverCount < minimumServers_) {
        return ValidationResult::error(
            "Global configuration must have at least " + std::to_string(minimumServers_) +
            " server block(s), but found " + std::to_string(serverCount));
    }

    return ValidationResult::success();
}