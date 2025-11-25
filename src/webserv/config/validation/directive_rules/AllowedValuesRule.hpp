#pragma once

#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <string> // for basic_string, string
#include <vector> // for vector

class AConfig;

class AllowedValuesRule : public AValidationRule
{
  public:
    explicit AllowedValuesRule(const std::vector<std::string> &allowedValues, bool requiresValue = true);

  private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config,
                                                 const std::string &directiveName) const override;
    std::vector<std::string> allowedValues_;
};