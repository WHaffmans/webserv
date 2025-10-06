#pragma once

#include <string>
#include <cstdint>

class ValidationResult
{
  public:
    enum class Type : uint8_t
    {
        SUCCESS,
        ERROR,
        WARNING
    };

    ~ValidationResult() = default;

    ValidationResult(const ValidationResult &other) = default;
    ValidationResult &operator=(const ValidationResult &other) = default;
    ValidationResult(ValidationResult &&other) noexcept = default;
    ValidationResult &operator=(ValidationResult &&other) noexcept = default;

    static ValidationResult success();
    static ValidationResult error(const std::string &message);
    static ValidationResult warning(const std::string &message);

    [[nodiscard]] bool isValidResult() const;
    [[nodiscard]] ValidationResult::Type getType() const ;
    [[nodiscard]] std::string getMessage() const;

  private:
    ValidationResult(Type type, std::string message = "");
    Type type_;
    std::string message_;
};