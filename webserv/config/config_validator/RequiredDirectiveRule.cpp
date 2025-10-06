#include "webserv/config/config_validator/ValidationResult.hpp"
#include "webserv/config/config_validator/RequiredDirectiveRule.hpp"
#include <webserv/config/config_validator/AValidationRule.hpp>
#include <webserv/config/AConfig.hpp>

RequiredDirectiveRule::RequiredDirectiveRule()
    : AValidationRule("RequiredDirectiveRule", "Ensures that a required directive is present in the configuration", true)
{
}

ValidationResult RequiredDirectiveRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    return ValidationResult::success();
}