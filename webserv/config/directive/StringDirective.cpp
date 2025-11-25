#include <webserv/config/directive/ADirective.hpp>      // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>  // for DirectiveValueType
#include <webserv/config/directive/StringDirective.hpp> // for IntDirective

#include <linux/limits.h>

StringDirective::StringDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void StringDirective::parse(const std::string &value)
{
    if (value.size() > PATH_MAX)
    {
        throw std::invalid_argument("StringDirective: string value exceeds maximum length");
    }
    value_ = value;
}

DirectiveValueType StringDirective::getValueType() const
{
    return value_;
}