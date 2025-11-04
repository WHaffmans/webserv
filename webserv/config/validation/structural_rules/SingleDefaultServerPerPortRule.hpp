#pragma once

#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>

class GlobalConfig;
class ServerConfig;

class SingleDefaultServerPerPortRule : public AStructuralValidationRule
{
  public:
    SingleDefaultServerPerPortRule();
    [[nodiscard]] ValidationResult validateGlobal(const GlobalConfig *config) const override;
};
