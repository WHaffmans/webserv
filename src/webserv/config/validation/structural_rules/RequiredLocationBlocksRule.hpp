#pragma once

#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule

#include <cstddef> // for size_t

class ServerConfig;

class RequiredLocationBlocksRule : public AStructuralValidationRule
{
  private:
    size_t minimumLocations_;

  public:
    explicit RequiredLocationBlocksRule(size_t minimumLocations = 1);

    ~RequiredLocationBlocksRule() override = default;

    RequiredLocationBlocksRule(const RequiredLocationBlocksRule &other) = delete;
    RequiredLocationBlocksRule &operator=(const RequiredLocationBlocksRule &other) = delete;
    RequiredLocationBlocksRule(RequiredLocationBlocksRule &&other) noexcept = delete;
    RequiredLocationBlocksRule &operator=(RequiredLocationBlocksRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validateServer(const ServerConfig *config) const override;
};