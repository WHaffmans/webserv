#pragma once

#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <string> // for string

class AConfig;

class RequiredDirectiveRule : public AValidationRule
{
  public:
    RequiredDirectiveRule();

  private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config,
                                                 const std::string &directiveName) const override;
};