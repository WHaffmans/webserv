#pragma once
#include <string>


class ValidationResult;

class IValidationRule
{
  public:
    virtual ~IValidationRule() = default;

    IValidationRule(const IValidationRule &other) = delete;
    IValidationRule &operator=(const IValidationRule &other) = delete;
    IValidationRule(IValidationRule &&other) noexcept = delete;
    IValidationRule &operator=(IValidationRule &&other) noexcept = delete;

    [[nodiscard]] virtual ValidationResult validate(const std::string &Adirective) const = 0;
    [[nodiscard]] virtual std::string getRuleName() const = 0;
    [[nodiscard]] virtual std::string getDescription() const = 0;
    
};