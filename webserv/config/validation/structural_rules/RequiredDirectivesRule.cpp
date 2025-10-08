#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/directive/DirectiveFactory.hpp>
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>
#include <webserv/config/validation/structural_rules/RequiredDirectivesRule.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/utils/utils.hpp>

#include <algorithm>
#include <cctype>

RequiredDirectivesRule::RequiredDirectivesRule()
    : AStructuralValidationRule("RequiredDirectivesRule", "Ensures required directives are present in each context")
{
}

ValidationResult validateUniversal(const AConfig *config, std::string configType)
{
    std::vector<std::string> missingDirectives;
    std::vector<std::string> prohibitedDirectives;

    for (const auto &directive : DirectiveFactory::supportedDirectives)
    {
        if (directive.context.find(std::toupper(configType[0])) != std::string::npos &&
            !config->owns(std::string(directive.name)))
        {
            missingDirectives.emplace_back(directive.name);
        }
        if ((directive.context.find(std::toupper(configType[0])) == std::string::npos &&
             directive.context.find(std::tolower(configType[0])) == std::string::npos) &&
            config->owns(std::string(directive.name)))
        {
            prohibitedDirectives.emplace_back(directive.name);
        }
    }

    if (missingDirectives.empty() && prohibitedDirectives.empty())
    {
        return ValidationResult::success();
    }

    std::string result;

    if (!missingDirectives.empty())
    {
        result += "Missing " + configType + " directive: ";
        result += utils::implode(missingDirectives, ", ");
    }
    if (!prohibitedDirectives.empty())
    {
        result += "Prohibited " + configType + " directive: ";
        result += utils::implode(prohibitedDirectives, ", ");
    }

    return ValidationResult::error(result);
}

ValidationResult RequiredDirectivesRule::validateGlobal(const GlobalConfig *config) const
{
    return validateUniversal(config, "global");
}

ValidationResult RequiredDirectivesRule::validateServer(const ServerConfig *config) const
{
    return validateUniversal(config, "server");
}

ValidationResult RequiredDirectivesRule::validateLocation(const LocationConfig *config) const
{
    return validateUniversal(config, "location");
}