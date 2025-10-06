#include <webserv/config/AConfig.hpp>
#include <webserv/config/config_validator/AValidationRule.hpp>
#include <webserv/config/config_validator/ValidationResult.hpp>
#include <webserv/config/directive/ADirective.hpp>
#include <webserv/log/Log.hpp>

AValidationRule::AValidationRule(std::string ruleName, std::string description, bool requiresValue)
    : ruleName_(std::move(ruleName)), description_(std::move(description)), requiresValue_(requiresValue)
{
}

ValidationResult AValidationRule::validate(const AConfig *config, const std::string &directiveName) const
{
    Log::trace(LOCATION);
    if (config == nullptr || directiveName.empty())
    {
        return ValidationResult::error("Invalid config or directive name");
    }

    if (!config->hasDirective(directiveName))
    {
        if (requiresValue_)
        {
            return ValidationResult::error("Directive '" + directiveName + "' is missing");
        }
        return ValidationResult::success();
    }
    return validateValue(config, directiveName);
}

std::string AValidationRule::getRuleName() const
{
    return ruleName_;
}

std::string AValidationRule::getDescription() const
{
    return description_;
}

bool AValidationRule::isRequired() const
{
    return requiresValue_;
}