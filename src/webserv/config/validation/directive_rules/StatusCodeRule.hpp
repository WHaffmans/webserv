#pragma once

#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <functional> // for function
#include <string>     // for string

class StatusCodeRule : public AValidationRule
{
  public:
    //TODO we could use this to not have the ttype parameter twice ( clang-lint)
    using ValidationPredicate = std::function<bool(int)>;

    StatusCodeRule(bool requiresValue, ValidationPredicate validator);

    ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const override;

  private:
    ValidationPredicate validator_;
};