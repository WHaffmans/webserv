#include <webserv/config/validation/structural_rules/RequiredLocationBlocksRule.hpp>

#include <webserv/config/ServerConfig.hpp>                                          // for ServerConfig
#include <webserv/config/validation/ValidationResult.hpp>                           // for ValidationResult
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule
#include <webserv/log/Log.hpp> // for LOCATION, Log

#include <string> // for allocator, operator+, char_traits, to_string, basic_string
#include <vector> // for vector

RequiredLocationBlocksRule::RequiredLocationBlocksRule(size_t minimumLocations)
    : AStructuralValidationRule("RequiredLocationBlocksRule", "Ensures server has at least "
                                                                  + std::to_string(minimumLocations)
                                                                  + " location block(s)"),
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
        return ValidationResult::error("Server block must have at least " + std::to_string(minimumLocations_)
                                       + " location block(s), but found " + std::to_string(locationCount));
    }

    return ValidationResult::success();
}