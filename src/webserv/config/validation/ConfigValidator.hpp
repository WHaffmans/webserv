#pragma once

#include <webserv/config/validation/ValidationEngine.hpp> // for ValidationEngine
#include <webserv/config/validation/ValidationResult.hpp> // for ValidationResult

#include <memory> // for unique_ptr
#include <vector> // for vector

class GlobalConfig;

class ConfigValidator
{
  public:
    ConfigValidator(const GlobalConfig *config);

    ConfigValidator(const ConfigValidator &other) = delete;
    ConfigValidator &operator=(const ConfigValidator &other) = delete;
    ConfigValidator(ConfigValidator &&other) noexcept = delete;
    ConfigValidator &operator=(ConfigValidator &&other) noexcept = delete;
    ~ConfigValidator() = default;

    [[nodiscard]] std::vector<ValidationResult> getValidationResults() const;
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;
    [[nodiscard]] bool hasErrors() const noexcept;

  private:
    std::unique_ptr<ValidationEngine> engine_;
};