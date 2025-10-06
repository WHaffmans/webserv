#pragma once

#include "webserv/config/GlobalConfig.hpp"
#include "webserv/config/config_validator/AValidationRule.hpp"
#include "webserv/config/config_validator/ValidationResult.hpp"
#include "webserv/config/LocationConfig.hpp"
#include "webserv/config/ServerConfig.hpp"
#include "webserv/config/AConfig.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

class ValidationEngine
{
    using RuleMap = std::map<std::string, std::vector<std::unique_ptr<AValidationRule>>>;

  public:
    ValidationEngine(const GlobalConfig *globalConfig = nullptr);
    ValidationEngine(const ValidationEngine &other) = delete;
    ValidationEngine &operator=(const ValidationEngine &other) = delete;
    ValidationEngine(ValidationEngine &&other) noexcept = delete;
    ValidationEngine &operator=(ValidationEngine &&other) noexcept = delete;
    ~ValidationEngine() = default;

    void addGlobalRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule);
    void addServerRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule);
    void addLocationRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule);

    void validate();

    [[nodiscard]] std::vector<ValidationResult> getValidationResults() const;
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;
    [[nodiscard]] bool hasErrors() const;

  private:
    static void addRule(RuleMap &ruleMap, const std::string &directiveName, std::unique_ptr<AValidationRule> rule);

    void validateConfig(RuleMap const &rules, const AConfig *config);
    void validateGlobalConfig(const GlobalConfig *config);
    void validateServerConfig(const ServerConfig *config);
    void validateLocationConfig(const std::string &path, const LocationConfig *config);
    RuleMap globalRules_;
    RuleMap serverRules_;
    RuleMap locationRules_;
    const GlobalConfig *globalConfig_;

    std::vector<ValidationResult> results_;
};