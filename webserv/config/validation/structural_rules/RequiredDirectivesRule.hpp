#pragma once

#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>

#include <cstddef>

class GlobalConfig;
class ServerConfig;
class LocationConfig;

class RequiredDirectivesRule : public AStructuralValidationRule
{
  public:
    explicit RequiredDirectivesRule();

    ~RequiredDirectivesRule() override = default;

    RequiredDirectivesRule(const RequiredDirectivesRule &other) = delete;
    RequiredDirectivesRule &operator=(const RequiredDirectivesRule &other) = delete;
    RequiredDirectivesRule(RequiredDirectivesRule &&other) noexcept = delete;
    RequiredDirectivesRule &operator=(RequiredDirectivesRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validateGlobal(const GlobalConfig *config) const override;
    [[nodiscard]] ValidationResult validateServer(const ServerConfig *config) const override;
    [[nodiscard]] ValidationResult validateLocation(const LocationConfig *config) const override;
};