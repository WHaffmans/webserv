#include <webserv/config/directive/SizeDirective.hpp>  // for SizeDirective

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType
#include <webserv/utils/utils.hpp>                     // for trim

#include <algorithm> // for __transform_fn, transform
#include <cctype>    // for tolower
#include <exception> // for exception
#include <stdexcept> // for invalid_argument

SizeDirective::SizeDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void SizeDirective::parse(const std::string &value)
{
    size_t multiplier = 1;
    size_t idx = 0;
    std::string number = value;
    std::ranges::transform(number, number.begin(), ::tolower);
    number = utils::trim(number);
    if (number.find_first_not_of("01234567890kmg") != std::string::npos)
    {
        throw std::invalid_argument("Invalid size directive: " + value + " in " + name_);
    }
    try
    {
        value_ = std::stoul(number, &idx);
    }
    catch (const std::exception &e)
    {
        throw std::invalid_argument("Invalid size directive: " + value + " in " + name_ + " - " + e.what());
    }
    if (idx == number.size())
    {
        return;
    }
    std::string suffix = number.substr(idx);
    if (suffix == "k")
    {
        multiplier = 1024UL;
    }
    else if (suffix == "m")
    {
        multiplier = 1024UL * 1024UL;
    }
    else if (suffix == "g")
    {
        multiplier = 1024UL * 1024UL * 1024UL;
    }
    else
    {
        throw std::invalid_argument("Invalid size directive: " + value + " in " + name_);
    }

    value_ *= multiplier;

    if (value_ > 1000000000UL) // 1 GB limit for sanity should be a constant
    {
        throw std::invalid_argument("Size directive too large: " + value + " in " + name_);
    }
}

DirectiveValueType SizeDirective::getValueType() const
{
    return value_;
}