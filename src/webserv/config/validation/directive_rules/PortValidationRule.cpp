#include <webserv/config/validation/directive_rules/PortValidationRule.hpp>

#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/log/Log.hpp> // for LOCATION, Log

#include <string> // for operator+, basic_string, allocator, char_traits, to_string, string

PortValidationRule::PortValidationRule(bool requiresValue)
    : AValidationRule("PortValidationRule", "Validates that the port number is within the valid range (1-65535)",
                      requiresValue)
{
}

ValidationResult PortValidationRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    Log::trace(LOCATION);
    const ADirective *directive = config->getDirective(directiveName);
    if (!directive->getValue().holds<int>())
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' does not hold an integer value");
    }

    int port = directive->getValue().get<int>();
    if (port < 1 || port > 65535)
    {
        return ValidationResult::error("Port number " + std::to_string(port) + " is out of valid range (1-65535)");
    }

    return ValidationResult::success();
}
