#pragma once

#include <webserv/config/validation/ValidationResult.hpp>

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
    AStructuralValidationRule(const std::string &ruleName, const std::string &description);

  public:
    virtual ~AStructuralValidationRule() = default;

    AStructuralValidationRule(const AStructuralValidationRule &other) = delete;
    AStructuralValidationRule &operator=(const AStructuralValidationRule &other) = delete;
    AStructuralValidationRule(AStructuralValidationRule &&other) noexcept = delete;
    AStructuralValidationRule &operator=(AStructuralValidationRule &&other) noexcept = delete;

    // Virtual validation methods - override as needed
    [[nodiscard]] virtual ValidationResult validateGlobal(const GlobalConfig *config) const;
    [[nodiscard]] virtual ValidationResult validateServer(const ServerConfig *config) const;

    [[nodiscard]] virtual ValidationResult validateLocation(const LocationConfig *config) const;

    [[nodiscard]] std::string getRuleName() const;

    [[nodiscard]] std::string getDescription() const;
};