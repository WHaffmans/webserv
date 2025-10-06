#pragma once

#include "webserv/config/validation/structural_rules/AStructuralValidationRule.hpp"

#include <cstddef>

class GlobalConfig;

class MinimumServerBlocksRule : public AStructuralValidationRule
{
private:
    size_t minimumServers_;

public:
    explicit MinimumServerBlocksRule(size_t minimumServers = 1);

    ~MinimumServerBlocksRule() override = default;

    MinimumServerBlocksRule(const MinimumServerBlocksRule &other) = delete;
    MinimumServerBlocksRule &operator=(const MinimumServerBlocksRule &other) = delete;
    MinimumServerBlocksRule(MinimumServerBlocksRule &&other) noexcept = delete;
    MinimumServerBlocksRule &operator=(MinimumServerBlocksRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validateGlobal(const GlobalConfig *config) const override;
};