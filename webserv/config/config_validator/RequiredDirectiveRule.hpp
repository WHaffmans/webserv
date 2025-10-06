#pragma once

#include "webserv/config/config_validator/AValidationRule.hpp"

#include <string>

class AConfig;

class RequiredDirectiveRule : public AValidationRule
{
  public:
    RequiredDirectiveRule();

  private:
    [[nodiscard]] ValidationResult validateValue(const AConfig *config,
                                                 const std::string &directiveName) const override;
};