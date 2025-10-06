#include "webserv/config/validation/structural_rules/RequiredLocationBlocksRule.hpp"

#include "webserv/config/ServerConfig.hpp"
#include "webserv/log/Log.hpp"

#include <string>

RequiredLocationBlocksRule::RequiredLocationBlocksRule(size_t minimumLocations)
    : AStructuralValidationRule("RequiredLocationBlocksRule", "Ensures server has at least " +
                                                                  std::to_string(minimumLocations) +
                                                                  " location block(s)"),
      minimumLocations_(minimumLocations)
{
}

ValidationResult RequiredLocationBlocksRule::validateServer(const ServerConfig *config) const
{
    Log::trace(LOCATION);

    if (config == nullptr)
    {
        return ValidationResult::error("Server config is null");
    }

    size_t locationCount = config->getLocationPaths().size();

    if (locationCount < minimumLocations_)
    {
        return ValidationResult::error("Server block must have at least " + std::to_string(minimumLocations_) +
                                       " location block(s), but found " + std::to_string(locationCount));
    }

    return ValidationResult::success();
}