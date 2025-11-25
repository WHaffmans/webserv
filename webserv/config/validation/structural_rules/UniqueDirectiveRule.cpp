#include <webserv/config/validation/structural_rules/UniqueDirectiveRule.hpp>

#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/log/Log.hpp>

UniqueDirectiveRule::UniqueDirectiveRule(const std::vector<std::string> &uniqueDirectives)
    : AStructuralValidationRule(
          "UniqueDirectiveRule",
          "Ensures certain directives are unique across locations when a minimum number of locations is met"),
      uniqueDirectives_(uniqueDirectives)
{
}

ValidationResult UniqueDirectiveRule::validateConfig(const AConfig *config) const
{
    for (const auto &directiveName : uniqueDirectives_)
    {
        size_t count = 0;
        for (const auto &directive : config->getDirectives())
        {
            if (directive->getName() == directiveName)
            {
                count++;
            }
        }
        if (count > 1)
        {
            Log::debug("Validation failed: Directive '" + directiveName + "' is not unique in config of type "
                       + config->getType());
            return ValidationResult::error("Directive '" + directiveName + "' is not unique in config of type "
                                           + config->getType());
        }
    }
    return ValidationResult::success();
}

ValidationResult UniqueDirectiveRule::validateServer(const ServerConfig *config) const
{
    Log::trace(LOCATION);
    return validateConfig(config);
}

ValidationResult UniqueDirectiveRule::validateLocation(const LocationConfig *config) const
{
    Log::trace(LOCATION);
    return validateConfig(config);
}

ValidationResult UniqueDirectiveRule::validateGlobal(const GlobalConfig *config) const
{
    Log::trace(LOCATION);
    return validateConfig(config);
}