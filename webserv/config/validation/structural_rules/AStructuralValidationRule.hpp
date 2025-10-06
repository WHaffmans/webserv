#pragma once

#include "webserv/config/validation/ValidationResult.hpp"

#include <string>

class GlobalConfig;
class ServerConfig;
class LocationConfig;

class AStructuralValidationRule
{
  private:
    std::string ruleName_;
    std::string description_;

  protected:
    AStructuralValidationRule(const std::string &ruleName, const std::string &description)
        : ruleName_(ruleName), description_(description)
    {
    }

  public:
    virtual ~AStructuralValidationRule() = default;

    AStructuralValidationRule(const AStructuralValidationRule &other) = delete;
    AStructuralValidationRule &operator=(const AStructuralValidationRule &other) = delete;
    AStructuralValidationRule(AStructuralValidationRule &&other) noexcept = delete;
    AStructuralValidationRule &operator=(AStructuralValidationRule &&other) noexcept = delete;

    // Virtual validation methods - override as needed
    [[nodiscard]] virtual ValidationResult validateGlobal(const GlobalConfig *config) const
    {
        static_cast<void>(config);          // Suppress unused parameter warning
        return ValidationResult::success(); // Default: no global validation
    }

    [[nodiscard]] virtual ValidationResult validateServer(const ServerConfig *config) const
    {
        static_cast<void>(config);          // Suppress unused parameter warning
        return ValidationResult::success(); // Default: no server validation
    }

    [[nodiscard]] virtual ValidationResult validateLocation(const LocationConfig *config) const
    {
        static_cast<void>(config);          // Suppress unused parameter warning
        return ValidationResult::success(); // Default: no location validation
    }

    // Non-virtual getters - set in constructor
    [[nodiscard]] std::string getRuleName() const { return ruleName_; }

    [[nodiscard]] std::string getDescription() const { return description_; }
};