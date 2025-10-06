#pragma once

#include "webserv/config/config_validator/AValidationRule.hpp"
#include "webserv/config/config_validator/ValidationResult.hpp"
#include <string>
#include <vector>

class AConfig;

class AllowedValuesRule : public AValidationRule
{
  public:
    explicit AllowedValuesRule(const std::vector<std::string> &allowedValues, bool requiresValue = true);

  private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const override;
    std::vector<std::string> allowedValues_;
};