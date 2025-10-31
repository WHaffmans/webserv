#include <webserv/config/directive/StringDirective.hpp> // for IntDirective

#include <webserv/config/directive/ADirective.hpp>      // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>  // for DirectiveValueType

StringDirective::StringDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void StringDirective::parse(const std::string &value)
{
    if (value.size() > 4096) //TODO: use PATH_MAX or NAME_MAX where appropriate 
    {
        throw std::invalid_argument("StringDirective: string value exceeds maximum length");
    }
    value_ = value;
}

DirectiveValueType StringDirective::getValueType() const
{
    return value_;
}