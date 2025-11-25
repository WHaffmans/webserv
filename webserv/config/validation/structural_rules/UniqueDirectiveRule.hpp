#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule

#include <cstddef> // for size_t
#include <vector>

class ServerConfig;

class UniqueDirectiveRule : public AStructuralValidationRule
{
  private:
    std::vector<std::string> uniqueDirectives_;

    ValidationResult validateConfig(const AConfig *config) const;

  public:
    explicit UniqueDirectiveRule(const std::vector<std::string> &uniqueDirectives);

    ~UniqueDirectiveRule() override = default;

    UniqueDirectiveRule(const UniqueDirectiveRule &other) = delete;
    UniqueDirectiveRule &operator=(const UniqueDirectiveRule &other) = delete;
    UniqueDirectiveRule(UniqueDirectiveRule &&other) noexcept = delete;
    UniqueDirectiveRule &operator=(UniqueDirectiveRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validateGlobal(const GlobalConfig *config) const override;

    [[nodiscard]] ValidationResult validateServer(const ServerConfig *config) const override;

    [[nodiscard]] ValidationResult validateLocation(const LocationConfig *config) const override;
};