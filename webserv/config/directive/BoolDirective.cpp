#include <webserv/config/directive/BoolDirective.hpp> // for IntDirective
#include <webserv/config/utils.hpp>                   // for trim

#include <algorithm>
#include <any>
#include <stdexcept>

void BoolDirective::parse(const std::string &arg)
{
    std::string value = arg;
    value = utils::trim(value);
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

DirectiveValueType BoolDirective::getValue() const
{
    return value_;
}