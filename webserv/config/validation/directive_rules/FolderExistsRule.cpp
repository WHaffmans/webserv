#include <webserv/config/validation/directive_rules/FolderExistsRule.hpp>

#include <webserv/config/directive/ADirective.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/utils/FileUtils.hpp>

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