#include <webserv/config/validation/ValidationEngine.hpp>

#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/GlobalConfig.hpp>   // for GlobalConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig
#include <webserv/config/validation/ValidationResult.hpp>                           // for ValidationResult
#include <webserv/config/validation/directive_rules/AValidationRule.hpp>            // for AValidationRule
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp> // for AStructuralValidationRule
#include <webserv/log/Log.hpp>                                                      // for Log, LOCATION

#include <algorithm> // for ranges::any_of
#include <exception> // for exception
#include <utility>   // for move, get

void ValidationEngine::addGlobalRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    addRule(globalRules_, directiveName, std::move(rule));
}

void ValidationEngine::addServerRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    addRule(serverRules_, directiveName, std::move(rule));
}

void ValidationEngine::addLocationRule(const std::string &directiveName, std::unique_ptr<AValidationRule> rule)
{
    addRule(locationRules_, directiveName, std::move(rule));
}

void ValidationEngine::addStructuralRule(std::unique_ptr<AStructuralValidationRule> rule)
{
    if (rule != nullptr)
    {
        structuralRules_.push_back(std::move(rule));
    }
}

void ValidationEngine::addRule(RuleMap &ruleMap, const std::string &directiveName,
                               std::unique_ptr<AValidationRule> rule)
{
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
bool ValidationEngine::hasErrors() const noexcept
{
    return std::ranges::any_of(results_, [](const ValidationResult &result) {
        return !result.isValidResult();
    });
}


void ValidationEngine::validateConfig(RuleMap const &rulesMap, const AConfig *config)
{
    Log::trace(LOCATION);
    for (const auto &[directiveName, rules] : rulesMap)
    {
        if (!config->has(directiveName))
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

    // Run location structural validation rules
    for (const auto &rule : structuralRules_)
    {
        try
        {
            ValidationResult result = rule->validateLocation(config);
            if (!result.isValidResult())
            {
                results_.push_back(result);
            }
        }
        catch (const std::exception &e)
        {
            results_.push_back(ValidationResult::error("Structural rule '" + rule->getRuleName()
                                                       + "' threw exception for location '" + path + "': " + e.what()));
        }
    }

    // Run location directive validation rules
    validateConfig(locationRules_, config);
}

void ValidationEngine::validateServerConfig(const ServerConfig *config)
{
    Log::trace(LOCATION);

    // Run server structural validation rules
    for (const auto &rule : structuralRules_)
    {
        try
        {
            ValidationResult result = rule->validateServer(config);
            if (!result.isValidResult())
            {
                results_.push_back(result);
            }
        }
        catch (const std::exception &e)
        {
            results_.push_back(
                ValidationResult::error("Structural rule '" + rule->getRuleName() + "' threw exception: " + e.what()));
        }
    }

    // Run server directive validation rules
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

    // Run global structural validation rules
    for (const auto &rule : structuralRules_)
    {
        try
        {
            ValidationResult result = rule->validateGlobal(config);
            if (!result.isValidResult())
            {
                results_.push_back(result);
            }
        }
        catch (const std::exception &e)
        {
            results_.push_back(
                ValidationResult::error("Structural rule '" + rule->getRuleName() + "' threw exception: " + e.what()));
        }
    }

    // Run global directive validation rules
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
    results_.clear(); // Clear previous results

    if (globalConfig_ != nullptr)
    {
        validateGlobalConfig(globalConfig_);
    }
    else
    {
        Log::warning("No GlobalConfig set for ValidationEngine; skipping validation.");
    }
}