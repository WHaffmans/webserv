#include <webserv/config/validation/ConfigValidator.hpp>

#include <webserv/config/validation/ValidationEngine.hpp>                     // for ValidationEngine
#include <webserv/config/validation/directive_rules/AValidationRule.hpp>      // for AValidationRule
#include <webserv/config/validation/directive_rules/AllowedValuesRule.hpp>    // for AllowedValuesRule
#include <webserv/config/validation/directive_rules/CgiExtValidationRule.hpp> // for CgiExtValidationRule
#include <webserv/config/validation/directive_rules/HostValidationRule.hpp>   // for HostValidationRule
#include <webserv/config/validation/directive_rules/PortValidationRule.hpp>   // for PortValidationRule
#include <webserv/config/validation/directive_rules/StatusCodeRule.hpp>
#include <webserv/config/validation/structural_rules/AStructuralValidationRule.hpp>  // for AStructuralValidationRule
#include <webserv/config/validation/structural_rules/MinimumServerBlocksRule.hpp>    // for MinimumServerBlocksRule
#include <webserv/config/validation/structural_rules/RequiredDirectivesRule.hpp>     // for RequiredDirectivesRule
#include <webserv/config/validation/structural_rules/RequiredLocationBlocksRule.hpp> // for RequiredLocationBlocksRule
#include <webserv/config/validation/structural_rules/SingleDefaultServerPerPortRule.hpp> // for SingleDefaultServerPerPortRule
#include <webserv/config/validation/structural_rules/UniqueDirectiveRule.hpp>
#include <webserv/config/validation/structural_rules/UniqueServerNamesRule.hpp> // for UniqueServerNamesRule
#include <webserv/log/Log.hpp>                                                  // for LOCATION, Log

#include <memory> // for unique_ptr, make_unique
#include <string> // for basic_string, string

class ValidationResult;

ConfigValidator::ConfigValidator(const GlobalConfig *config) : engine_(std::make_unique<ValidationEngine>(config))
{
    Log::trace(LOCATION);
    // TODO seems weird that errorpages are only checked globally and per location, not per server block? check the rest
    // aswell

    /*Structural Rules*/
    engine_->addStructuralRule(std::make_unique<MinimumServerBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<RequiredLocationBlocksRule>(1));
    engine_->addStructuralRule(std::make_unique<UniqueServerNamesRule>());
    engine_->addStructuralRule(std::make_unique<RequiredDirectivesRule>());
    engine_->addStructuralRule(std::make_unique<SingleDefaultServerPerPortRule>());
    engine_->addStructuralRule(std::make_unique<UniqueDirectiveRule>(std::vector<std::string>{
        "index", "listen", "host", "server_name", "root", "allowed_methods", "autoindex", "cgi_enabled", "upload_store",
        "client_max_body_size", "cgi_timeout", "redirect", "timeout", "42_tester"}));

    /*Global Directive Rules*/
    engine_->addServerRule("error_page", std::make_unique<StatusCodeRule>(false, [](int statusCode) {
                               return statusCode >= 100 && statusCode <= 599;
                           }));

    /*Server Directive Rules*/
    engine_->addServerRule("listen", std::make_unique<PortValidationRule>());
    engine_->addServerRule("host", std::make_unique<HostValidationRule>());
    // Folder existence validation disabled - paths are relative to server runtime directory

    /*Location Directive Rules*/
    engine_->addLocationRule("allowed_methods",
                             std::make_unique<AllowedValuesRule>(
                                 std::vector<std::string>{"GET", "POST", "DELETE", "PUT", "HEAD", "OPTIONS"}, false));
    engine_->addLocationRule("error_page", std::make_unique<StatusCodeRule>(false, [](int statusCode) {
                                 return statusCode >= 100 && statusCode <= 599;
                             }));
    engine_->addLocationRule("redirect", std::make_unique<StatusCodeRule>(false, [](int statusCode) {
                                 // Only accept valid redirect codes: 301, 302, 303, 307, 308
                                 return statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307
                                        || statusCode == 308;
                             }));
    // Folder existence validation disabled - paths are relative to server runtime directory
    engine_->addLocationRule("cgi_handler", std::make_unique<CgiExtValidationRule>(false));

    // TODO: Add a validation rule for redirect

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

bool ConfigValidator::hasErrors() const noexcept
{
    return engine_->hasErrors();
}
