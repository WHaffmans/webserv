#include <webserv/config/validation/directive_rules/AValidationRule.hpp>

#include <webserv/config/AConfig.hpp>                     // for AConfig
#include <webserv/config/validation/ValidationResult.hpp> // for ValidationResult

#include <utility> // for move

AValidationRule::AValidationRule(std::string ruleName, std::string description, bool requiresValue)
    : ruleName_(std::move(ruleName)), description_(std::move(description)), requiresValue_(requiresValue)
{
}

ValidationResult AValidationRule::validate(const AConfig *config, const std::string &directiveName) const
{
    if (config == nullptr || directiveName.empty())
    {
        return ValidationResult::error("Invalid config or directive name");
    }

    if (!config->has(directiveName))
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