#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule

#include <map>    // for map
#include <memory> // for unique_ptr
#include <string> // for basic_string, string, operator<=>
#include <vector> // for vector

class AConfig;
class GlobalConfig;
class LocationConfig;
class ServerConfig;

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