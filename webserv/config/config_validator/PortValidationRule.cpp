#include "webserv/config/config_validator/PortValidationRule.hpp"

#include "webserv/config/AConfig.hpp"
#include "webserv/config/config_validator/ValidationResult.hpp"
#include "webserv/config/directive/ADirective.hpp"
#include "webserv/config/directive/DirectiveValue.hpp"
#include "webserv/log/Log.hpp"

#include <string> // for string, basic_string, operator+, char_traits

PortValidationRule::PortValidationRule(bool requiresValue)
    : AValidationRule("PortValidationRule", "Validates that the port number is within the valid range (1-65535)", requiresValue)
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
