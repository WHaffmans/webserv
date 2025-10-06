#include <webserv/config/validation/directive_rules/AllowedValuesRule.hpp>

#include <webserv/config/AConfig.hpp>                                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>                       // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>                   // for DirectiveValue
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <functional> // for identity
#include <ranges>     // for __find_fn, find
#include <string>     // for basic_string, allocator, operator+, char_traits, string, operator==
#include <utility>    // for move
#include <vector>     // for vector

AllowedValuesRule::AllowedValuesRule(const std::vector<std::string> &allowedValues, bool requiresValue)
    : AValidationRule("AllowedValuesRule", "Ensures that the directive's value is within the allowed set",
                      requiresValue),
      allowedValues_(allowedValues)
{
}

ValidationResult AllowedValuesRule::validateValue(const AConfig *config, const std::string &directiveName) const
{
    const ADirective *directive = config->getDirective(directiveName);
    if (directive == nullptr)
    {
        return ValidationResult::error("Directive '" + directiveName + "' is missing");
    }

    auto checkValueAllowed = [&](const std::string &val) -> bool {
        return std::ranges::find(allowedValues_, val) != allowedValues_.end();
    };

    auto makeErrorMsg = [&](const std::string &val) -> std::string {
        return "Value '" + val + "' of directive '" + directiveName + "' is not in the allowed set";
    };

    if (directive->getValue().holds<std::string>())
    {
        const std::string &value = directive->getValue().get<std::string>();

        if (!checkValueAllowed(value))
        {
            return ValidationResult::error(makeErrorMsg(value));
        }
    }
    else if (directive->getValue().holds<std::vector<std::string>>())
    {
        const std::vector<std::string> &values = directive->getValue().get<std::vector<std::string>>();
        for (const std::string &val : values)
        {
            if (!checkValueAllowed(val))
            {
                return ValidationResult::error(makeErrorMsg(val));
            }
        }
    }
    else
    {
        return ValidationResult::error("Directive '" + directiveName + "' has an unsupported value type");
    }

    return ValidationResult::success();
}