#include <webserv/config/validation/directive_rules/HostValidationRule.hpp>

#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/utils/utils.hpp> // for split

#include <cctype> // for isdigit
#include <string> // for basic_string, allocator, char_traits, operator+, string, stoi
#include <vector> // for vector

HostValidationRule::HostValidationRule(bool requiresValue)
    : AValidationRule("HostValidationRule", "Validates that the host is a valid domain name", requiresValue)
{
}

bool isValidIPv4(const std::string &ip)
{
    std::vector<std::string> parts = utils::split(ip, '.');
    if (parts.size() != 4)
    {
        return false;
    }
    for (const std::string &part : parts)
    {
        if (part.empty() || part.size() > 3)
        {
            return false;
        }
        for (char c : part)
        {
            if (std::isdigit(c) < 0)
            {
                return false;
            }
        }
        int num = std::stoi(part);
        if (num < 0 || num > 255)
        {
            return false;
        }
        if (part.size() > 1 && part[0] == '0')
        {
            return false;
        }
    }
    return true;
}

ValidationResult HostValidationRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    const ADirective *directive = config->getDirective(directiveName);

    if (!directive->getValue().holds<std::string>())
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' does not hold a string value");
    }

    auto host = directive->getValue().get<std::string>();

    if (!isValidIPv4(host))
    {
        return ValidationResult::error("Host '" + host + "' is not a valid IPv4 address");
    }

    return ValidationResult::success();
}