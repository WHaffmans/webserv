#pragma once

#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <string> // for string

class StatusCodeRule : public AValidationRule
{
  public:
    StatusCodeRule(bool requiresValue);

    ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const override;
};