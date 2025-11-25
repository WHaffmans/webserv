#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>

#include <webserv/config/validation/ValidationResult.hpp> // for ValidationResult

AStructuralValidationRule::AStructuralValidationRule(const std::string &ruleName, const std::string &description)
    : ruleName_(ruleName), description_(description)
{
}

ValidationResult AStructuralValidationRule::validateGlobal(const GlobalConfig *config) const
{
    static_cast<void>(config);
    return ValidationResult::success();
}

ValidationResult AStructuralValidationRule::validateServer(const ServerConfig *config) const
{
    static_cast<void>(config);
    return ValidationResult::success();
}

ValidationResult AStructuralValidationRule::validateLocation(const LocationConfig *config) const
{
    static_cast<void>(config);
    return ValidationResult::success();
}

std::string AStructuralValidationRule::getRuleName() const
{
    return ruleName_;
}

std::string AStructuralValidationRule::getDescription() const
{
    return description_;
}