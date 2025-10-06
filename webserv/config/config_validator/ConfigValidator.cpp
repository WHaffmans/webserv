#include "webserv/config/config_validator/ConfigValidator.hpp"

#include "webserv/config/GlobalConfig.hpp"
#include "webserv/config/config_validator/AllowedValuesRule.hpp"
#include "webserv/config/config_validator/PortValidationRule.hpp"
#include "webserv/log/Log.hpp"
#include <string>

ConfigValidator::ConfigValidator(const GlobalConfig *config)
    : engine_(std::make_unique<ValidationEngine>(config))
{
    Log::trace(LOCATION);

    engine_->addServerRule("listen", std::make_unique<PortValidationRule>());
    engine_->addLocationRule("allowed_methods", std::make_unique<AllowedValuesRule>(std::vector<std::string>{"GET", "POST", "DELETE"}));
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

