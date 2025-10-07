#include <webserv/config/validation/directive_rules/CgiExtValidationRule.hpp>

#include <webserv/config/AConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/utils/FileUtils.hpp>

#include <algorithm>
#include <vector>

CgiExtValidationRule::CgiExtValidationRule(bool requiresValue)
    : AValidationRule("CgiExt", "Ensure CGI extension is valid", requiresValue)
{
}

bool isAllowedCGIExtension(const std::string &extension)
{
    static const std::vector<std::string> allowedExtensions = {".php", ".py"};
    return std::ranges::any_of(allowedExtensions, [&extension](const auto &it) { return extension == it; });
}

ValidationResult CgiExtValidationRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    const ADirective *directive = config->getDirective(directiveName);
    if (!directive->getValue().holds<std::vector<std::string>>())
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' does not hold a string value");
    }

    auto cgiExt = directive->getValue().get<std::vector<std::string>>();
    if (cgiExt.size() != 2)
    {
        return ValidationResult::error("Directive '" + directive->getName() +
                                       "' has invalid format, expected extension and path");
    }
    auto extension = std::string(cgiExt[0]);
    auto path = std::string(cgiExt[1]);
    if (extension.empty() || extension[0] != '.')
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' has invalid extension '" + extension +
                                       "'");
    }
    if (!isAllowedCGIExtension(extension))
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' has unsupported extension '" +
                                       extension + "'");
    }
    if (!FileUtils::isFile(path))
    {
        return ValidationResult::error("Directive '" + directive->getName() + "' has invalid path '" + path + "'");
    }
    return ValidationResult::success();
}