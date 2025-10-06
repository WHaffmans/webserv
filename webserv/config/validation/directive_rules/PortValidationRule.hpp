#pragma once

#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <string> // for string

class AConfig;

class PortValidationRule : public AValidationRule
{
  public:
    PortValidationRule(bool requiresValue = true);

  private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config,
                                                 const std::string &directiveName) const override;
};