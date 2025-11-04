#include <webserv/config/AConfig.hpp>                                               // for AConfig
#include <webserv/config/GlobalConfig.hpp>                                          // for GlobalConfig
#include <webserv/config/LocationConfig.hpp>                                        // for LocationConfig
#include <webserv/config/ServerConfig.hpp>                                          // for ServerConfig
#include <webserv/config/directive/DirectiveFactory.hpp>                            // for DirectiveFactory
#include <webserv/config/validation/ValidationResult.hpp>                           // for ValidationResult
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule
#include <webserv/config/validation/structural_rules/RequiredDirectivesRule.hpp>
#include <webserv/utils/utils.hpp> // for implode

#include <array>  // for array
#include <cctype> // for toupper, tolower
#include <string> // for basic_string, allocator, char_traits, string, operator+
#include <vector> // for vector

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
        if (directive.context.find(static_cast<char>(std::toupper(configType[0]))) != std::string::npos
            && !config->owns(std::string(directive.name)))
        {
            missingDirectives.emplace_back(directive.name);
        }
        if ((directive.context.find(static_cast<char>(std::toupper(static_cast<unsigned char>(configType[0]))))
                 == std::string::npos
             && directive.context.find(static_cast<char>(std::tolower(static_cast<unsigned char>(configType[0]))))
                    == std::string::npos)
            && config->owns(std::string(directive.name)))
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

// TODO ! WTF is happening here, we should look at the hard coded validation rules again

ValidationResult RequiredDirectivesRule::validateGlobal(const GlobalConfig *config) const
{
    // No globally required directives at this time; only prohibit invalid ones.
    std::vector<std::string> prohibited;
    for (const auto &info : DirectiveFactory::supportedDirectives)
    {
        bool allowedInGlobal
            = (info.context.find('G') != std::string::npos) || (info.context.find('g') != std::string::npos);
        if (!allowedInGlobal && config->owns(std::string(info.name)))
        {
            prohibited.emplace_back(info.name);
        }
    }
    if (prohibited.empty())
    {
        return ValidationResult::success();
    }
    std::string result = "Prohibited global directive: ";
    result += utils::implode(prohibited, ", ");
    return ValidationResult::error(result);
}

ValidationResult RequiredDirectivesRule::validateServer(const ServerConfig *config) const
{
    // TODO missing directives are hard coded, it doesnt look at the table?
    // Only a minimal set is required for server blocks; other directives are optional.
    std::vector<std::string> missing;
    if (!config->owns("listen"))
    {
        missing.emplace_back("listen");
    }
    if (!config->owns("root"))
    {
        missing.emplace_back("root");
    }

    // Detect prohibited directives (those not allowed in server context but present)
    std::vector<std::string> prohibited;
    for (const auto &info : DirectiveFactory::supportedDirectives)
    {
        bool allowedInServer
            = (info.context.find('S') != std::string::npos) || (info.context.find('s') != std::string::npos);
        if (!allowedInServer && config->owns(std::string(info.name)))
        {
            prohibited.emplace_back(info.name);
        }
    }

    if (missing.empty() && prohibited.empty())
    {
        return ValidationResult::success();
    }

    std::string result;
    if (!missing.empty())
    {
        result += "Missing server directive: ";
        result += utils::implode(missing, ", ");
    }
    if (!prohibited.empty())
    {
        result += "Prohibited server directive: ";
        result += utils::implode(prohibited, ", ");
    }
    return ValidationResult::error(result);
}

ValidationResult RequiredDirectivesRule::validateLocation(const LocationConfig *config) const
{
    // No required directives in a location; only prohibit invalid ones.
    std::vector<std::string> prohibited;
    for (const auto &info : DirectiveFactory::supportedDirectives)
    {
        bool allowedInLocation
            = (info.context.find('L') != std::string::npos) || (info.context.find('l') != std::string::npos);
        if (!allowedInLocation && config->owns(std::string(info.name)))
        {
            prohibited.emplace_back(info.name);
        }
    }
    if (prohibited.empty())
    {
        return ValidationResult::success();
    }
    std::string result = "Prohibited location directive: ";
    result += utils::implode(prohibited, ", ");
    return ValidationResult::error(result);
}
