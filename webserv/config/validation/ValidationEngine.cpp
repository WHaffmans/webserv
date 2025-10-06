#include <webserv/config/validation/ValidationEngine.hpp>

#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/GlobalConfig.hpp>   // for GlobalConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig
#include <webserv/config/validation/ValidationResult.hpp>                // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp> // for AValidationRule
#include <webserv/log/Log.hpp>                                           // for Log, LOCATION

#include <utility> // for move, get

void ValidationEngine::addGlobalRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    Log::trace(LOCATION);
    addRule(globalRules_, directiveName, std::move(rule));
}

void ValidationEngine::addServerRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    Log::trace(LOCATION);
    addRule(serverRules_, directiveName, std::move(rule));
}

void ValidationEngine::addLocationRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    Log::trace(LOCATION);
    addRule(locationRules_, directiveName, std::move(rule));
}

void ValidationEngine::addRule(RuleMap &ruleMap, const std::string &directiveName,
                               std::unique_ptr<AValidationRule> rule)
{
    Log::trace(LOCATION);
    ruleMap[directiveName].emplace_back(std::move(rule));
}

std::vector<ValidationResult> ValidationEngine::getValidationResults() const
{
    return results_;
}

std::vector<ValidationResult> ValidationEngine::getErrors() const
{
    std::vector<ValidationResult> errors;
    for (const auto &result : results_)
    {
        if (!result.isValidResult())
        {
            errors.push_back(result);
        }
    }
    return errors;
}

std::vector<ValidationResult> ValidationEngine::getWarnings() const
{
    std::vector<ValidationResult> warnings;
    for (const auto &result : results_)
    {
        if (result.getType() == ValidationResult::Type::WARNING)
        {
            warnings.push_back(result);
        }
    }
    return warnings;
}

bool ValidationEngine::hasErrors() const
{
    for (const auto &result : results_)
    {
        if (!result.isValidResult())
        {
            return true;
        }
    }
    return false;
}

void ValidationEngine::validateConfig(RuleMap const &rulesMap, const AConfig *config)
{
    Log::trace(LOCATION);
    for (const auto &[directiveName, rules] : rulesMap)
    {
        if (!config->hasDirective(directiveName))
        {
            // Check if any rule requires the directive
            for (const auto &rule : rules)
            {
                if (rule->isRequired())
                {
                    ValidationResult result = rule->validate(config, directiveName);
                    if (!result.isValidResult())
                    {
                        results_.push_back(result); // Only failures
                    }
                }
            }
            continue; // Directive not present, skip to next
        }
        // Validate each rule for this directive
        for (const auto &rule : rules)
        {
            ValidationResult result = rule->validate(config, directiveName); // ✅
            if (!result.isValidResult())
            {
                results_.push_back(result); // Only failures
            }
        }
    }
}

void ValidationEngine::validateLocationConfig(const std::string &path, const LocationConfig *config)
{
    Log::trace(LOCATION);
    validateConfig(locationRules_, config);
}

void ValidationEngine::validateServerConfig(const ServerConfig *config)
{
    Log::trace(LOCATION);
    validateConfig(serverRules_, config);
    for (const auto &path : config->getLocationPaths())
    {
        const LocationConfig *locationConfig = config->getLocation(path);
        if (locationConfig != nullptr)
        {
            validateLocationConfig(path, locationConfig);
        }
    }
}

void ValidationEngine::validateGlobalConfig(const GlobalConfig *config)
{
    Log::trace(LOCATION);
    validateConfig(globalRules_, config);
    for (const auto *serverConfig : config->getServerConfigs())
    {
        validateServerConfig(serverConfig);
    }
}

ValidationEngine::ValidationEngine(const GlobalConfig *globalConfig) : globalConfig_(globalConfig)
{
    Log::trace(LOCATION);
}

void ValidationEngine::validate()
{
    Log::trace(LOCATION);
    if (globalConfig_ != nullptr)
    {
        validateGlobalConfig(globalConfig_);
    }
    else
    {
        Log::warning("No GlobalConfig set for ValidationEngine; skipping validation.");
    }
}