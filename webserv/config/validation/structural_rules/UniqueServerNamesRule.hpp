#pragma once

#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule

class GlobalConfig;

class UniqueServerNamesRule : public AStructuralValidationRule
{
  public:
    UniqueServerNamesRule();

    ~UniqueServerNamesRule() override = default;

    UniqueServerNamesRule(const UniqueServerNamesRule &other) = delete;
    UniqueServerNamesRule &operator=(const UniqueServerNamesRule &other) = delete;
    UniqueServerNamesRule(UniqueServerNamesRule &&other) noexcept = delete;
    UniqueServerNamesRule &operator=(UniqueServerNamesRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validateGlobal(const GlobalConfig *config) const override;
};