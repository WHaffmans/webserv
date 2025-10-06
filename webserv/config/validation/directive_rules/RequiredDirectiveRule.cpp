#include <webserv/config/validation/directive_rules/RequiredDirectiveRule.hpp>

#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

RequiredDirectiveRule::RequiredDirectiveRule()
    : AValidationRule("RequiredDirectiveRule", "Ensures that a required directive is present in the configuration",
                      true)
{
}

ValidationResult RequiredDirectiveRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    return ValidationResult::success();
}