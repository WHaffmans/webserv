#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <string> // for string

class FolderExistsRule : public AValidationRule
{
  public:
    FolderExistsRule(bool requiresValue);

    ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const override;
};
