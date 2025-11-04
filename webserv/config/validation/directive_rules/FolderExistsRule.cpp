#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/config/validation/directive_rules/FolderExistsRule.hpp>
#include <webserv/log/Log.hpp>         // for Log
#include <webserv/utils/FileUtils.hpp> // for isDirectory, joinPath

#include <filesystem> // for path

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

    // if (!FileUtils::isDirectory(folderPath))
    // {
    //     return ValidationResult::error(folderPath + " is not a valid directory");
    // }

    // TODO check if we change the basedir = cwd stuff if this is coreect?
    // originally it just returns success when the previous tests didnt fail
    // Try multiple resolution strategies:
    // 1) As provided (relative to current working directory)
    // 2) Relative to config base directory
    // 3) Relative to the parent of the config base directory (common for test setups)
    std::vector<std::string> candidates;
    candidates.emplace_back(folderPath);
    candidates.emplace_back(config->resolvePath(folderPath));
    std::filesystem::path base(config->getBaseDir());
    std::string parentBase = base.parent_path().string();
    if (!parentBase.empty())
    {
        candidates.emplace_back(FileUtils::joinPath(parentBase, folderPath));
    }

    for (const auto &p : candidates)
    {
        if (!p.empty() && FileUtils::isDirectory(p))
        {
            return ValidationResult::success();
        }
    }

    // Keep original path in the error message to match tester expectations
    return ValidationResult::error("invalid root path: " + folderPath);
}