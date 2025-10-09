#include <webserv/config/validation/directive_rules/FolderExistsRule.hpp>

#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/log/Log.hpp>         // for Log
#include <webserv/utils/FileUtils.hpp> // for isDirectory

FolderExistsRule::FolderExistsRule(bool requiresValue)
    : AValidationRule("FolderExists", "Ensures the specified folder exists", requiresValue)
{
}

ValidationResult FolderExistsRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    const ADirective *directive = config->getDirective(directiveName);
    if (!directive->getValue().holds<std::string>())
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' does not hold a string value");
    }

    auto folderPath = directive->getValue().get<std::string>();
    Log::debug("Validating folder exists: " + folderPath);
    if (!FileUtils::isDirectory(folderPath))
    {
        return ValidationResult::error(folderPath + " is not a valid directory");
    }
    return ValidationResult::success();
}