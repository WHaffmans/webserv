#pragma once

#include "webserv/config/config_validator/IValidationRule.hpp"
class PortValidationRule : public IValidationRule
{
  public:
    PortValidationRule() = default;
    ~PortValidationRule() override = default;

    PortValidationRule(const PortValidationRule &other) = delete;
    PortValidationRule &operator=(const PortValidationRule &other) = delete;
    PortValidationRule(PortValidationRule &&other) noexcept = delete;
    PortValidationRule &operator=(PortValidationRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validate(const std::string &value) const override;
    [[nodiscard]] std::string getRuleName() const override { return "PortValidationRule"; }
    [[nodiscard]] std::string getDescription() const override { return "Validates that a port number is between 1 and 65535"; }
};