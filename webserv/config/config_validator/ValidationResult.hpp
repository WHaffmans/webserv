#pragma once

#include <string>

class ValidationResult
{
  public:
    ~ValidationResult() = default;

    ValidationResult(const ValidationResult &other) = delete;
    ValidationResult &operator=(const ValidationResult &other) = delete;
    ValidationResult(ValidationResult &&other) noexcept = default;
    ValidationResult &operator=(ValidationResult &&other) noexcept = default;

    static ValidationResult success();
    static ValidationResult error(const std::string &message);

  private:
    ValidationResult(bool isValid, std::string errorMessage = "");
    bool isValid;
    std::string errorMessage;
};