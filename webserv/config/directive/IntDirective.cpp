#include <webserv/config/directive/IntDirective.hpp>   // for IntDirective

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType

IntDirective::IntDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void IntDirective::parse(const std::string &value)
{
    value_ = std::stoi(value); // TODO: check parsing
}

DirectiveValueType IntDirective::getValueType() const
{
    return value_;
}