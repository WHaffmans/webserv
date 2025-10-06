#pragma once
#include <string>


class ValidationResult;
class ADirective;
class AConfig;

class AValidationRule
{
  public:
    virtual ~AValidationRule() = default;

    AValidationRule(const AValidationRule &other) = delete;
    AValidationRule &operator=(const AValidationRule &other) = delete;
    AValidationRule(AValidationRule &&other) noexcept = delete;
    AValidationRule &operator=(AValidationRule &&other) noexcept = delete;

    [[nodiscard]] ValidationResult validate(const AConfig *config, const std::string &directiveName) const;
    [[nodiscard]] bool isRequired() const;
    [[nodiscard]] virtual ValidationResult validateValue(const AConfig *config, const std::string &directiveName) const = 0;
    [[nodiscard]] std::string getRuleName() const;
    [[nodiscard]] std::string getDescription() const;

  protected:
    AValidationRule(std::string ruleName, std::string description, bool requiresValue = true);

  private:
    std::string ruleName_;
    std::string description_;
    bool requiresValue_;
};