#include <webserv/config/directive/BoolDirective.hpp> // for IntDirective
#include <webserv/config/utils.hpp>                   // for trim

#include <algorithm> // for __transform_fn, transform
#include <cctype>    // for tolower
#include <stdexcept> // for invalid_argument

BoolDirective::BoolDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void BoolDirective::parse(const std::string &arg)
{
    std::string value = utils::trim(arg);
    std::ranges::transform(value, value.begin(), ::tolower);
    if (value == "true" || value == "1" || value == "on" || value == "yes")
    {
        value_ = true;
    }
    else if (value == "false" || value == "0" || value == "off" || value == "no")
    {
        value_ = false;
    }
    else
    {
        throw std::invalid_argument("Invalid boolean value: " + value);
    }
}

DirectiveValueType BoolDirective::getValueType() const
{
    return value_;
}