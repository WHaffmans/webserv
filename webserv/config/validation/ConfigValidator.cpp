#include <webserv/config/validation/ConfigValidator.hpp>

#include <webserv/config/validation/ValidationEngine.hpp>                   // for ValidationEngine
#include <webserv/config/validation/directive_rules/AValidationRule.hpp>    // for AValidationRule
#include <webserv/config/validation/directive_rules/AllowedValuesRule.hpp>  // for AllowedValuesRule
#include <webserv/config/validation/directive_rules/PortValidationRule.hpp> // for PortValidationRule
#include <webserv/config/validation/structural_rules/StructuralRules.hpp>   // for structural rules
#include <webserv/log/Log.hpp>                                              // for LOCATION, Log

#include <string> // for basic_string, string

ConfigValidator::ConfigValidator(const GlobalConfig *config) : engine_(std::make_unique<ValidationEngine>(config))
{
    Log::trace(LOCATION);

    /*Structural Rules*/
    engine_->addStructuralRule(std::make_unique<MinimumServerBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<RequiredLocationBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<UniqueServerNamesRule>());

    /*Global Directive Rules*/

    /*Server Directive Rules*/
    engine_->addServerRule("listen", std::make_unique<PortValidationRule>());

    /*Location Directive Rules*/
    engine_->addLocationRule("allowed_methods",
                             std::make_unique<AllowedValuesRule>(std::vector<std::string>{"GET", "POST", "DELETE"}));

    engine_->validate();
}

std::vector<ValidationResult> ConfigValidator::getValidationResults() const
{
    return engine_->getValidationResults();
}

std::vector<ValidationResult> ConfigValidator::getErrors() const
{
    return engine_->getErrors();
}

std::vector<ValidationResult> ConfigValidator::getWarnings() const
{
    return engine_->getWarnings();
}

bool ConfigValidator::hasErrors() const
{
    return engine_->hasErrors();
}
