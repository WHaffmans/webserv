#pragma once

#include "webserv/config/config_validator/AValidationRule.hpp"
#include <string>

class AConfig;
class PortValidationRule : public AValidationRule
{
  public:
    PortValidationRule(bool requiresValue = true);

    private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const override;
};