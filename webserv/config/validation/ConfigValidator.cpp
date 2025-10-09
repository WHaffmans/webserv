#include <webserv/config/validation/ConfigValidator.hpp>

#include <webserv/config/validation/ValidationEngine.hpp>                            // for ValidationEngine
#include <webserv/config/validation/directive_rules/AValidationRule.hpp>             // for AValidationRule
#include <webserv/config/validation/directive_rules/AllowedValuesRule.hpp>           // for AllowedValuesRule
#include <webserv/config/validation/directive_rules/CgiExtValidationRule.hpp>        // for CgiExtValidationRule
#include <webserv/config/validation/directive_rules/FolderExistsRule.hpp>            // for FolderExistsRule
#include <webserv/config/validation/directive_rules/HostValidationRule.hpp>          // for HostValidationRule
#include <webserv/config/validation/directive_rules/PortValidationRule.hpp>          // for PortValidationRule
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>  // for AStructuralValidationRule
#include <webserv/config/validation/structural_rules/MinimumServerBlocksRule.hpp>    // for MinimumServerBlocksRule
#include <webserv/config/validation/structural_rules/RequiredDirectivesRule.hpp>     // for RequiredDirectivesRule
#include <webserv/config/validation/structural_rules/RequiredLocationBlocksRule.hpp> // for RequiredLocationBlocksRule
#include <webserv/config/validation/structural_rules/UniqueServerNamesRule.hpp>      // for UniqueServerNamesRule
#include <webserv/log/Log.hpp>                                                       // for LOCATION, Log

#include <memory> // for unique_ptr, make_unique
#include <string> // for basic_string, string

class ValidationResult;

ConfigValidator::ConfigValidator(const GlobalConfig *config) : engine_(std::make_unique<ValidationEngine>(config))
{
    Log::trace(LOCATION);

    /*Structural Rules*/
    engine_->addStructuralRule(std::make_unique<MinimumServerBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<RequiredLocationBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<UniqueServerNamesRule>());
    engine_->addStructuralRule(std::make_unique<RequiredDirectivesRule>());

    /*Global Directive Rules*/

    /*Server Directive Rules*/
    engine_->addServerRule("listen", std::make_unique<PortValidationRule>());
    engine_->addServerRule("host", std::make_unique<HostValidationRule>());
    engine_->addServerRule("root", std::make_unique<FolderExistsRule>(false));

    /*Location Directive Rules*/
    engine_->addLocationRule("allowed_methods",
                             std::make_unique<AllowedValuesRule>(std::vector<std::string>{"GET", "POST", "DELETE"}));
    engine_->addLocationRule("root", std::make_unique<FolderExistsRule>(true));
    engine_->addLocationRule("cgi_ext", std::make_unique<CgiExtValidationRule>(false));

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
