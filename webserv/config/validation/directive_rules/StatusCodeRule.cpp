#include <webserv/config/validation/directive_rules/StatusCodeRule.hpp>

#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/utils/FileUtils.hpp> // for isFile

#include <string>
#include <utility>

StatusCodeRule::StatusCodeRule(bool requiresValue)
    : AValidationRule("CgiExt", "Ensure CGI extension is valid", requiresValue)
{
}

ValidationResult StatusCodeRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    const ADirective *directive = config->getDirective(directiveName);
    if (!directive->getValue().holds<std::pair<int, std::string>>())
    {
        return ValidationResult::error("Directive '" + directive->getName()
                                       + "' does not hold a int-string pair value");
    }

    auto value = directive->getValue().get<std::pair<int, std::string>>();
    int statusCode = value.first;
    if (statusCode < 100 || statusCode > 599)
    {
        return ValidationResult::error("Directive '" + directive->getName()
                                       + "' has invalid status code: " + std::to_string(statusCode));
    }

    return ValidationResult::success();
}